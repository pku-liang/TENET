#include "dataflow.h"
#include <ctime>
#include <filesystem>

using namespace std;
using namespace TENET;
using path = filesystem::path;

#define Debug 0
#define Test_Switch 0
#define VERBOSE 0

int main(int argc, char* argv[])
{
    shared_ptr<ISL_Context> context{make_shared<ISL_Context>(stdout)};

    if (argc != 4) {
        fprintf(stderr, "Arg number incorrect! Expect 4, got %d.\n", argc);
        return 1;
    }

    path   dir{argv[1]};
    string name{argv[2]};
    int    n{atoi(argv[3])};

    auto _pe_file             = dir / path("pe_array") / path(name + ".p");
    auto _mapping_file        = dir / path("mapping") / path(name + ".m");
    auto _statement_file_base = (dir / path("statement") / path(name + "_")).string();

    PEArray pe(context);
    if (!pe.Load(_pe_file.c_str())) {
        fprintf(stderr, "Load PE %s failed\n", _pe_file.c_str());
        return 1;
    }
    Mapping mp(context);
    if (!mp.Load(_mapping_file.c_str())) {
        fprintf(stderr, "Load Mapping %s failed\n", _mapping_file.c_str());
        return 1;
    }

    int latency = 0;
    int total   = 0;
    int unique  = 0;

    for (int i = 0; i != n; ++i) {
        auto _statement_file = _statement_file_base + to_string(i) + ".s";

        Statement st(context);
        if (!st.Load(_statement_file.c_str())) {
            fprintf(stderr, "Load Statement %s failed\n", _statement_file.c_str());
            return 1;
        }
        auto [input, output] = st.GetTensorList();

        Dataflow df(move(st), move(pe.copy()), move(mp.copy()));

        isl_union_map* space_time_to_neighbor = df.MapSpaceTimeToNeighbor();

        int ingress_delay     = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
        int egress_delay      = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor));
        int computation_delay = df.GetComputationDelay();

        latency += max(ingress_delay, max(egress_delay, computation_delay));

        for (auto& iter : input) {
            total += df.GetTotalVolume(iter, AccessType::READ);
            unique += df.GetUniqueVolume(iter, AccessType::READ, isl_union_map_copy(space_time_to_neighbor));
        }

        for (auto& iter : output) {
            total += df.GetTotalVolume(iter, AccessType::WRITE);
            unique += df.GetUniqueVolume(iter, AccessType::WRITE, isl_union_map_copy(space_time_to_neighbor));
        }
        isl_union_map_free(space_time_to_neighbor);
    }

    printf("%d\n", latency);
    printf("%d\n", total);
    printf("%d\n", unique);

    return 0;
}
