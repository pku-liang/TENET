#include "dataflow.h"
#include "config.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <unistd.h>

using namespace std;
using namespace TENET;
using path = filesystem::path;

#define Debug 0
#define VERBOSE 0

static const char help_text[] = "Usage: tenet [options]\n"
                                "Options:\n"
                                "  -h, --help:                 Display this information.\n"
                                "  -e, --experiment <arg>:     Path of the expriment file or a directory.\n"
                                "                              If <arg> is a file, run the experiment file.\n"
                                "                              If <arg> is a directory, run all\n"
                                "                              experiment_? files inside of the directory. \n"
                                "                              When this argument is specified, -p, -m and\n"
                                "                              -s are ignored.\n"
                                "  -d, --experiment_dir <dir>: Base directory for files in the experiment.\n"
                                "  -p, --pe_array <file>:      Path of the pe array file.\n"
                                "  -m, --mapping <file>:       Path of the mapping file.\n"
                                "  -s, --statement <file>:     Path of the statement file.\n"
                                "  -o, --output <file>:        Path of the output CSV file.\n"
                                "  --all:                      Print all infomation.\n"
                                "Contact semiwaker@pku.edu.cn for bugs\n";

struct OutputState {
    OutputState(ofstream& f, bool v = false) : verbose(v), fout(f) {}
    bool      verbose{false};
    bool      titled{false};
    ofstream& fout;
};

void DataflowAnalysis(shared_ptr<ISL_Context> context, const char* expName, const char* _pe_file,
                      const char* _statement_file, const char* _mapping_file, OutputState& out)
{
    PEArray pe(context);
    if (!pe.Load(_pe_file)) {
        fprintf(stderr, "Load PE %s failed\n", _pe_file);
        return;
    }
    Statement st(context);
    if (!st.Load(_statement_file)) {
        fprintf(stderr, "Load Statement %s failed\n", _statement_file);
        return;
    }
    Mapping mp(context);
    if (!mp.Load(_mapping_file)) {
        fprintf(stderr, "Load Mapping %s failed\n", _mapping_file);
        return;
    }
    auto [input, output] = st.GetTensorList();
    Dataflow df(move(st), move(pe), move(mp));  // st, pe and mp is moved into df, DONT USE THEM AGAIN!

    isl_union_map* space_time_to_neighbor = df.MapSpaceTimeToNeighbor();

    if (!out.titled) {
        out.titled = true;
        out.fout << "Experiment,";
        out.fout << "Delay In, Delay Out, Delay Computation,";
        out.fout << "Active PE Num, Active PE Average,";
        if (out.verbose) {
            for (auto& iter : input) {
                out.fout << iter << " Reuse Factor,";
                out.fout << iter << " Temporal Reuse Factor,";
                out.fout << iter << " Spatial Reuse Factor,";
                out.fout << iter << " Total Volume,";
                out.fout << iter << " Unique Volume,";
            }
            for (auto& iter : output) {
                out.fout << iter << " Reuse Factor,";
                out.fout << iter << " Temporal Reuse Factor,";
                out.fout << iter << " Spatial Reuse Factor,";
                out.fout << iter << " Total Volume,";
                out.fout << iter << " Unique Volume,";
            }
        }
        out.fout << endl;
    }

    if (out.fout.is_open())
        out.fout << expName << ",";

    int ingress_delay     = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
    int egress_delay      = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor));
    int computation_delay = df.GetComputationDelay();
    fprintf(stdout, "Delay: In: %d; Out: %d; Com: %d\n", ingress_delay, egress_delay, computation_delay);
    if (out.fout.is_open())
        out.fout << ingress_delay << "," << egress_delay << "," << computation_delay << ",";

    int    dsize     = df.GetActivePENum();
    double avg_dsize = df.GetAverageActivePENum();
    fprintf(stdout, "Active PE Num: %d; Average: %.2f\n", dsize, avg_dsize);
    if (out.fout.is_open())
        out.fout << dsize << "," << avg_dsize << ",";

    if (out.verbose) {
        for (auto& iter : input) {
            double reuse_factor = df.GetReuseFactor(iter, AccessType::READ, isl_union_map_copy(space_time_to_neighbor));
            double temporal_reuse = df.GetTemporalReuseVolume(iter, AccessType::READ);
            double spatial_reuse  = df.GetSpatialReuseVolume(iter, AccessType::READ, NULL);
            int    totalVolume    = df.GetTotalVolume(iter, AccessType::READ);
            int uniqueVolume = df.GetUniqueVolume(iter, AccessType::READ, isl_union_map_copy(space_time_to_neighbor));
            fprintf(stdout, "Input Tensor: %s\n Reuse Factor: %.2f\n", iter.c_str(), reuse_factor);
            fprintf(stdout, " temporal: %f\n spatial: %f\n", temporal_reuse, spatial_reuse);
            fprintf(stdout, " TotalVolume: %d\n UniqueVolume: %d\n", totalVolume, uniqueVolume);
            if (out.fout.is_open()) {
                out.fout << setprecision(2) << reuse_factor << ",";
                out.fout << temporal_reuse << ",";
                out.fout << spatial_reuse << ",";
                out.fout << totalVolume << "," << uniqueVolume << ",";
            }
        }
        for (auto& iter : output) {
            double reuse_factor =
                df.GetReuseFactor(iter, AccessType::WRITE, isl_union_map_copy(space_time_to_neighbor));
            double temporal_reuse = df.GetTemporalReuseVolume(iter, AccessType::WRITE);
            double spatial_reuse  = df.GetSpatialReuseVolume(iter, AccessType::WRITE, NULL);
            int    totalVolume    = df.GetTotalVolume(iter, AccessType::WRITE);
            int uniqueVolume = df.GetUniqueVolume(iter, AccessType::WRITE, isl_union_map_copy(space_time_to_neighbor));
            fprintf(stdout, "Output Tensor: %s\n Reuse Factor: %.2f\n", iter.c_str(), reuse_factor);
            fprintf(stdout, " temporal: %f\n spatial: %f\n", temporal_reuse, spatial_reuse);
            fprintf(stdout, " TotalVolume: %d\n UniqueVolume: %d\n", totalVolume, uniqueVolume);
            if (out.fout.is_open()) {
                out.fout << setprecision(2) << reuse_factor << ",";
                out.fout << temporal_reuse << ",";
                out.fout << spatial_reuse << ",";
                out.fout << totalVolume << "," << uniqueVolume << ",";
            }
        }
    }
    if (out.fout.is_open())
        out.fout << endl;

    isl_union_map_free(space_time_to_neighbor);
}

int experiment(shared_ptr<ISL_Context> context, path experiment_file, path experiment_dir, OutputState& out)
{
    fprintf(stdout, "Experiment %s\n", experiment_file.filename().c_str());

    string   mapping, pe_array, statement;
    int      inner_cycle;
    ifstream experiment(experiment_file);
    if (!experiment.is_open()) {
        fprintf(stdout, "Experiment file %s fail to open\n", experiment_file.c_str());
        return 0;
    }

    experiment >> mapping >> pe_array >> statement;
    experiment.close();

    path m = experiment_dir / path(mapping);
    path p = experiment_dir / path(pe_array);
    path s = experiment_dir / path(statement);
    DataflowAnalysis(context, experiment_file.c_str(), p.c_str(), s.c_str(), m.c_str(), out);

    fprintf(stdout, "\n");
    return 0;
}

struct CLI_Field {
    int   all;
    int   help;
    char* pe_array_file;
    char* mapping_file;
    char* statement_file;
    char* experiment_file;
    char* experiment_dir;
    char* output_file;
};

int main(int argc, char* argv[])
{
    CLI_Field field{0, 0, NULL, NULL, NULL, NULL, NULL, NULL};

    const struct option long_opts[] = {
        {"all",            no_argument,       &field.all,  1  },
        {"help",           no_argument,       &field.help, 1  },
        {"pe_array",       required_argument, NULL,        'p'},
        {"statement",      required_argument, NULL,        's'},
        {"mapping",        required_argument, NULL,        'm'},
        {"experiment",     required_argument, NULL,        'e'},
        {"experiment_dir", required_argument, NULL,        'd'},
        {"output",         required_argument, NULL,        'o'},
        {NULL,             0,                 NULL,        0  }
    };

    int op;
    while (~(op = getopt_long(argc, argv, "hp:m:s:e:d:o:", long_opts, NULL))) {
        switch (op) {
        case 'h': field.help = true; break;
        case 'p': field.pe_array_file = optarg; break;
        case 'm': field.mapping_file = optarg; break;
        case 's': field.statement_file = optarg; break;
        case 'e': field.experiment_file = optarg; break;
        case 'd': field.experiment_dir = optarg; break;
        case 'o': field.output_file = optarg; break;
        case 0: break;
        default:
            fprintf(stderr, "Invalid arguments %s\n", argv[optind - 1]);
            field.help = true;
            break;
        }
    }

    if (field.help
        || ((!field.pe_array_file || !field.mapping_file || !field.statement_file) && !field.experiment_file)) {
        printf("%s\n", help_text);
        return 0;
    }

    shared_ptr<ISL_Context> context{make_shared<ISL_Context>(stdout)};

    ofstream fout;
    if (field.output_file)
        fout.open(field.output_file);

    OutputState out{fout, field.all};

    if (field.experiment_file) {
        if (!field.experiment_dir) {
            field.experiment_dir[0] = '.';
            field.experiment_dir[1] = 0;
        }
        path exp_file{field.experiment_file};
        path exp_dir{field.experiment_dir};
        if (filesystem::is_directory(exp_file)) {
            for (auto& f : filesystem::directory_iterator(exp_file))
                experiment(context, f.path(), exp_dir, out);
        }
        else
            experiment(context, exp_file, exp_dir, out);
    }
    else {
        DataflowAnalysis(context, "experiment", field.pe_array_file, field.statement_file, field.mapping_file, out);
    }

    return 0;
}
