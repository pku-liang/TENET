#include"dataflow.h"

#define N_experiment 15

void DataflowAnalysis(const char* _pe_file, const char* _statement_file,
	const char* _mapping_file)
{
	PEArray pe;
	if (!pe.Load(_pe_file))
	{
		fprintf(stderr, "Load PE failed\n");
		return;
	}
	Statement st;
	if (!st.Load(_statement_file))
	{
		fprintf(stderr, "Load Statement failed\n");
		return;
	}
	Mapping mp;
	if (!mp.Load(_mapping_file))
	{
		fprintf(stderr, "Load Mapping failed\n");
		return;
	}
	pe.PrintInfo();
	st.PrintInfo();
	mp.PrintInfo();
	Dataflow df(st, pe, mp);
	vector<string> input, output;
	st.GetTensorList(input, output);
	isl_union_map *space_time_to_neighbor = df.MapSpaceTimeToNeighbor();
#ifdef DEBUG
	fprintf(stdout, "Space Domain:\n");
	p = isl_printer_print_union_set(p, df.GetSpaceDomain());
	fprintf(stdout, "Reuse Map:\n");
	p = isl_printer_print_union_map(p, space_time_to_neighbor);
	p = isl_printer_end_line(p);
#endif
	for (auto iter = input.begin(); iter != input.end(); iter++)
	{
		double reuse_factor = df.GetReuseFactor(*iter, READ, isl_union_map_copy(space_time_to_neighbor));
		double temporal_reuse = df.GetTemporalReuseVolume(*iter, READ);
		double spatial_reuse = df.GetSpatialReuseVolume(*iter, READ);
		fprintf(stdout, "Input Tensor: %s\n Reuse Factor: %.2f\n", iter->c_str(), reuse_factor);
		fprintf(stdout, " temporal(per instance): %f\n spatial(per instance): %f\n",temporal_reuse,spatial_reuse);
		// fprintf(stdout, "%s %.2f ", iter->c_str(), reuse_factor);
		// fprintf(stdout, "%f %f\n", temporal_reuse, spatial_reuse);
	}
	for (auto iter = output.begin(); iter != output.end(); iter++)
	{
		double reuse_factor = df.GetReuseFactor(*iter, WRITE, isl_union_map_copy(space_time_to_neighbor));
		double temporal_reuse = df.GetTemporalReuseVolume(*iter, WRITE);
		double spatial_reuse = df.GetSpatialReuseVolume(*iter, WRITE);
		fprintf(stdout, "Output Tensor: %s\n Reuse Factor: %.2f\n", iter->c_str(), reuse_factor);
		fprintf(stdout, " temporal(per instance): %f\n spatial(per instance): %f\n",temporal_reuse,spatial_reuse);
		// fprintf(stdout, "%s %.2f ", iter->c_str(), reuse_factor);
		// fprintf(stdout, "%f %f\n", temporal_reuse, spatial_reuse);
	}

	int ingress_delay = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
	int egress_delay = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor));
	int computation_delay = df.GetComputationDelay();
	fprintf(stdout, "Delay(cycles): In: %d; Out: %d; Com: %d\n", ingress_delay, egress_delay, computation_delay);
	int dsize = df.GetActivePENum();
	double avg_dsize = df.GetAverageActivePENum();
	fprintf(stdout, "Active PE Num: %d; Average: %.2f\n", dsize, avg_dsize);
	isl_union_map_free(space_time_to_neighbor);
}

int experiment(string experiment_file) {
#ifdef ABSOLUTE_PATH
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
	getline(experiment,mapping);
	getline(experiment,pe_array);
	getline(experiment,statement);
	// experiment >> inner_cycle;
	// cout<<"----------     "<<experiment_file<<"     ---------------"<<endl;
	DataflowAnalysis((prefix+pe_array).c_str(),(prefix+statement).c_str(),(prefix+mapping).c_str());
	experiment.close();
	return 0;
}

int main(int argc, char * argv[])
{
	ctx = isl_ctx_alloc();
	p = isl_printer_to_file(ctx, stdout);
	p = isl_printer_set_output_format(p, ISL_FORMAT_ISL);
#ifdef EXPERIMENT
#ifdef ABSOLUTE_PATH
	string experiment_prefix = "/mnt/d/WSL/sttAnalysis/sttAnalysis/experi/experiment_";
#else
	string experiment_prefix = "experi/experiment_";
#endif
	for (int i=0; i<=N_experiment; i++) {
		string experiment_file = experiment_prefix+to_string(i);
		experiment(experiment_file);
	}
#else
	const char* pe_file = argv[1];
	const char* mapping_file = argv[2];
	const char* statement_file = argv[3];
	DataflowAnalysis(pe_file, statement_file, mapping_file);
#endif
	isl_printer_free(p);
	isl_ctx_free(ctx);
	return 0;
}
