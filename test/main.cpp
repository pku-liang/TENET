#include"dataflow.h"
#include<ctime>

using namespace std;
using namespace TENET;

#define N_experiment 5
#define EXPERIMENT 1
#define Debug 0
#define Verbose 0
#define ABSOLUTE_PATH 0
#define Test_Switch 0
#define Compute_delay_and_pe_activity 1

void DataflowAnalysis(
	shared_ptr<ISL_Context> context,
	const char* _pe_file,
	const char* _statement_file,
	const char* _mapping_file)
{
	PEArray pe(context);
	if (!pe.Load(_pe_file))
	{
		fprintf(stderr, "Load PE failed\n");
		return;
	}
	Statement st(context);
	if (!st.Load(_statement_file))
	{
		fprintf(stderr, "Load Statement failed\n");
		return;
	}
	Mapping mp(context);
	if (!mp.Load(_mapping_file))
	{
		fprintf(stderr, "Load Mapping failed\n");
		return;
	}
#if Debug
	pe.PrintInfo();
	st.PrintInfo();
	mp.PrintInfo();
#endif
	auto [input, output] = st.GetTensorList();
	Dataflow df(move(st), move(pe), move(mp)); // st, pe and mp is moved into df, DONT USE THEM AGAIN!

	isl_union_map *space_time_to_neighbor = df.MapSpaceTimeToNeighbor();
#if DEBUG
	fprintf(stdout, "Space Domain:\n");
	p = isl_printer_print_union_set(p, df.GetSpaceDomain());
	fprintf(stdout, "Reuse Map:\n");
	p = isl_printer_print_union_map(p, space_time_to_neighbor);
	p = isl_printer_end_line(p);
#endif
	for (auto& iter : input)
	{
		double reuse_factor =
			df.GetReuseFactor(iter, AccessType::READ, isl_union_map_copy(space_time_to_neighbor));
		// double temporal_reuse = df.GetTemporalReuseVolume(*iter, READ);
		// double spatial_reuse = df.GetSpatialReuseVolume(*iter, READ, NULL);
#if Verbose
		fprintf(stdout, "Input Tensor: %s\n Reuse Factor: %.2f\n", iter->c_str(), reuse_factor);
		fprintf(stdout, " temporal: %f\n spatial: %f\n",temporal_reuse,spatial_reuse);
#else
		// fprintf(stdout, "%s %.2f ", iter->c_str(), reuse_factor);
		// fprintf(stdout, "%f %f\n", temporal_reuse, spatial_reuse);
#endif
	}
	for (auto& iter : output)
	{
		double reuse_factor =
			df.GetReuseFactor(iter, AccessType::WRITE, isl_union_map_copy(space_time_to_neighbor));
		// double temporal_reuse = df.GetTemporalReuseVolume(*iter, WRITE);
		// double spatial_reuse = df.GetSpatialReuseVolume(*iter, WRITE, NULL);
#if Verbose
		fprintf(stdout, "Output Tensor: %s\n Reuse Factor: %.2f\n", iter->c_str(), reuse_factor);
		fprintf(stdout, " temporal: %f\n spatial: %f\n",temporal_reuse,spatial_reuse);
#else
		// fprintf(stdout, "%s %.2f ", iter->c_str(), reuse_factor);
		// fprintf(stdout, "%f %f\n", temporal_reuse, spatial_reuse);
#endif
	}
#if Compute_delay_and_pe_activity
	int ingress_delay = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
	int egress_delay = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor));
	int computation_delay = df.GetComputationDelay();
	fprintf(stdout, "Delay: In: %d; Out: %d; Com: %d\n", ingress_delay, egress_delay, computation_delay);
	int dsize = df.GetActivePENum();
	double avg_dsize = df.GetAverageActivePENum();
	fprintf(stdout, "Active PE Num: %d; Average: %.2f\n", dsize, avg_dsize);
#endif
	isl_union_map_free(space_time_to_neighbor);
}

int experiment(shared_ptr<ISL_Context> context, string experiment_file) {
#if ABSOLUTE_PATH
	string prefix = "/mnt/d/WSL/sttAnalysis/sttAnalysis/data/";
#else
	string prefix = "data/";
#endif
	
	string mapping, pe_array, statement;
	int inner_cycle;
	ifstream experiment(experiment_file);
	if (!experiment.is_open())
	{
		fprintf(stdout, "Experiment file %s fail to open\n", experiment_file.c_str());
		return 0;
	}
	clock_t start,end;
	start=clock();
	getline(experiment,mapping);
	getline(experiment,pe_array);
	getline(experiment,statement);
	// cout<<"----------     "<<experiment_file<<"     ---------------"<<endl;
	DataflowAnalysis(
		context,
		(prefix+pe_array).c_str(),
		(prefix+statement).c_str(),
		(prefix+mapping).c_str());
	end=clock();
	double endtime=(double)(end-start)/CLOCKS_PER_SEC;
	// printf("Total time:%f\n",endtime);
#if Test_Switch
	if(pe_array.compare("pe_array/systolic.p")==0)
		pe_array="pe_array/switch.p";
	cout<<"--------use switch 2d array as alternative--------"<<endl;
	DataflowAnalysis((prefix+pe_array).c_str(),(prefix+statement).c_str(),(prefix+mapping).c_str());
#endif
	experiment.close();
	return 0;
}

int main(int argc, char * argv[])
{
	shared_ptr<ISL_Context> context{make_shared<ISL_Context>(stdout)};
#if EXPERIMENT
#if ABSOLUTE_PATH
	string experiment_prefix = "/mnt/d/WSL/sttAnalysis/sttAnalysis/experi/experiment_";
#else
	string experiment_prefix = "data/alexnet/experiment_";
#endif
	for (int i=1; i<=N_experiment; i++) {
		string experiment_file = experiment_prefix+to_string(i);
		experiment(context, experiment_file);
	}
#else
	const char* pe_file = "/mnt/d/WSL/sttAnalysis/sttAnalysis/data/pe_array/systolic.p";
	const char* mapping_file = "/mnt/d/WSL/sttAnalysis/sttAnalysis/data/mapping/conv2d_eyeriss2d.m";
	const char* statement_file = "/mnt/d/WSL/sttAnalysis/sttAnalysis/data/statement/conv2d.s";
	DataflowAnalysis(context, pe_file, statement_file, mapping_file, 3);
#endif
	return 0;
}
