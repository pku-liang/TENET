#include"dataflow.h"
#include "config.h"
#include <ctime>
#include <filesystem>

using namespace std;
using namespace TENET;
using path = filesystem::path;

#define Debug 0
#define Test_Switch 0
#define VERBOSE 0 

void DataflowAnalysis(
	shared_ptr<ISL_Context> context,
	const char* _pe_file,
	const char* _statement_file,
	const char* _mapping_file)
{
	PEArray pe(context);
	if (!pe.Load(_pe_file))
	{
		fprintf(stderr, "Load PE %s failed\n", _pe_file);
		return;
	}
	Statement st(context);
	if (!st.Load(_statement_file))
	{
		fprintf(stderr, "Load Statement %s failed\n", _statement_file);
		return;
	}
	Mapping mp(context);
	if (!mp.Load(_mapping_file))
	{
		fprintf(stderr, "Load Mapping %s failed\n", _mapping_file);
		return;
	}
	auto [input, output] = st.GetTensorList();
	Dataflow df(move(st), move(pe), move(mp)); // st, pe and mp is moved into df, DONT USE THEM AGAIN!

	isl_union_map *space_time_to_neighbor = df.MapSpaceTimeToNeighbor();

#if VERBOSE
	for (auto& iter : input)
	{
		double reuse_factor =
			df.GetReuseFactor(iter, AccessType::READ, isl_union_map_copy(space_time_to_neighbor));
		double temporal_reuse = df.GetTemporalReuseVolume(iter, AccessType::READ);
		double spatial_reuse = df.GetSpatialReuseVolume(iter, AccessType::READ, NULL);
		fprintf(stdout, "Input Tensor: %s\n Reuse Factor: %.2f\n", iter.c_str(), reuse_factor);
		fprintf(stdout, " temporal: %f\n spatial: %f\n",temporal_reuse,spatial_reuse);
	}
	for (auto& iter : output)
	{
		double reuse_factor =
			df.GetReuseFactor(iter, AccessType::WRITE, isl_union_map_copy(space_time_to_neighbor));
		double temporal_reuse = df.GetTemporalReuseVolume(iter, AccessType::WRITE);
		double spatial_reuse = df.GetSpatialReuseVolume(iter, AccessType::WRITE, NULL);
		fprintf(stdout, "Output Tensor: %s\n Reuse Factor: %.2f\n", iter.c_str(), reuse_factor);
		fprintf(stdout, " temporal: %f\n spatial: %f\n",temporal_reuse,spatial_reuse);
	}
#endif


	int ingress_delay = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
	int egress_delay = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor));
	int computation_delay = df.GetComputationDelay();
	fprintf(stdout, "Delay: In: %d; Out: %d; Com: %d\n", ingress_delay, egress_delay, computation_delay);

	int dsize = df.GetActivePENum();
	double avg_dsize = df.GetAverageActivePENum();
	fprintf(stdout, "Active PE Num: %d; Average: %.2f\n", dsize, avg_dsize);

	isl_union_map_free(space_time_to_neighbor);
}

int experiment(shared_ptr<ISL_Context> context, path experiment_file) {
	fprintf(stdout, "Experiment %s\n", experiment_file.filename().c_str());
	string prefix = "data/";

	string mapping, pe_array, statement;
	int inner_cycle;
	ifstream experiment(experiment_file);
	if (!experiment.is_open())
	{
		fprintf(stdout, "Experiment file %s fail to open\n", experiment_file.c_str());
		return 0;
	}

	experiment >> mapping >> pe_array >> statement;
	DataflowAnalysis(
		context,
		(prefix+pe_array).c_str(),
		(prefix+statement).c_str(),
		(prefix+mapping).c_str());

#if Test_Switch
	if(pe_array.compare("pe_array/systolic.p")==0)
		pe_array="pe_array/switch.p";
	cout<<"--------use switch 2d array as alternative--------"<<endl;
	DataflowAnalysis((prefix+pe_array).c_str(),(prefix+statement).c_str(),(prefix+mapping).c_str());
#endif
	experiment.close();
	fprintf(stdout, "\n");
	return 0;
}

int main(int argc, char * argv[])
{
	shared_ptr<ISL_Context> context{make_shared<ISL_Context>(stdout)};
	auto dir = filesystem::directory_entry(path("./data") / EXPERIMENT_PREFIX / path("experiment"));
	for (auto&f : filesystem::directory_iterator(dir))
		experiment(context, f.path());
	return 0;
}
