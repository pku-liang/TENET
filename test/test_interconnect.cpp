#include"dataflow.h"

#define N_experiment 15

void DataflowAnalysis(const char* _pe_file, const char* _statement_file,
	const char* _mapping_file, int inner_cycle=1)
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
#ifdef DEBUG	
	pe.PrintInfo();
	st.PrintInfo();
	mp.PrintInfo();
#endif
	Dataflow df(st, pe, mp);
	vector<string> input, output;
	st.GetTensorList(input, output);
	isl_union_map *space_time_to_neighbor = NULL;
	isl_union_map *spatial_stt_neighbor = NULL;
	if(strcmp(_pe_file,"data/pe_array/torus.p")==0) {
		PEArray pe2;
		pe2.Load("data/pe_array/systolic.p");
		Dataflow df2(st, pe2, mp);
		isl_union_map *space_time_to_neighbor_PureSpatial = df2.MapSpaceTimeToNeighbor(1,true,0,true,false);	
		isl_union_map *space_time_to_neighbor_PureTemporal = df.MapSpaceTimeToNeighbor(0,false,1,false,false);
		isl_union_map *space_time_to_neighbor_Mixed = df.MapSpaceTimeToNeighbor(1,false,1,false,false);
		isl_union_map *space_time_to_neighbor_Temporal = df.MapSpaceTimeToNeighbor(1,true,1,false,false);
		spatial_stt_neighbor = isl_union_map_union(isl_union_map_copy(space_time_to_neighbor_PureSpatial), space_time_to_neighbor_Mixed);
		// space_time_to_neighbor = isl_union_map_union(space_time_to_neighbor_PureTemporal, isl_union_map_copy(spatial_stt_neighbor));
		
		space_time_to_neighbor = isl_union_map_union(space_time_to_neighbor_PureSpatial, space_time_to_neighbor_Temporal);
		// printf("use method 1\n");
		// p = isl_printer_print_union_map(p, space_time_to_neighbor);
		// p = isl_printer_end_line(p);

		// printf("use method 2\n");
		// p = isl_printer_print_union_map(p, alternative);
		// p = isl_printer_end_line(p);
		
	}else {
		space_time_to_neighbor = df.MapSpaceTimeToNeighbor();
		spatial_stt_neighbor = df.MapSpaceTimeToNeighbor(1, false, 1, true, false);
	}
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
		double spatial_reuse = df.GetSpatialReuseVolume(*iter, READ, isl_union_map_copy(spatial_stt_neighbor));
		double unique_volume = (double) df.GetUniqueVolume(*iter, READ, isl_union_map_copy(space_time_to_neighbor));
		int ingress_delay = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor), *iter);
		int computation_delay = df.GetComputationDelay();
		fprintf(stdout, "%s %.2f %.2f %.4f %d %d\n", iter->c_str(), reuse_factor, spatial_reuse * df.GetPENum(), unique_volume/computation_delay, ingress_delay, computation_delay);
	}
	for (auto iter = output.begin(); iter != output.end(); iter++)
	{
		double reuse_factor = df.GetReuseFactor(*iter, WRITE, isl_union_map_copy(space_time_to_neighbor));
		double spatial_reuse = df.GetSpatialReuseVolume(*iter, WRITE, isl_union_map_copy(spatial_stt_neighbor));
		double unique_volume = (double) df.GetUniqueVolume(*iter, WRITE, isl_union_map_copy(space_time_to_neighbor));
		int egress_delay = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor), *iter);
		int computation_delay = df.GetComputationDelay();
		fprintf(stdout, "%s %.2f %.2f %.4f %d %d\n", iter->c_str(), reuse_factor, spatial_reuse * df.GetPENum(), unique_volume/computation_delay, egress_delay, computation_delay);
	}
	isl_union_map_free(space_time_to_neighbor);
	isl_union_map_free(spatial_stt_neighbor);
}


int experiment(string experiment_kernel) {
	string exp_prefix = "interconnect_experiment/";
	string pe_array, mapping, statement;
	ifstream experiment_file(exp_prefix+experiment_kernel);
	if (!experiment_file.is_open())
	{
		fprintf(stdout, "Experiment file %s fail to open\n", experiment_kernel.c_str());
		return 0;
	}
	int n_dims;
	experiment_file >> statement >> n_dims;
	for (int i=0; i<n_dims; i++) {
		int n_mapping, n_interconnect;
		string Nd_interconnect;
		experiment_file >> n_mapping >> Nd_interconnect;
		ifstream interconnect_file(exp_prefix+Nd_interconnect);
		interconnect_file >> n_interconnect;
		string interconnects[10];
		for (int j=0; j<n_interconnect; j++) {
			interconnect_file >> interconnects[j];
		}
		interconnect_file.close();
		for (int j=0; j<n_mapping; j++) {
			experiment_file >> mapping;
			for (int k=0; k<n_interconnect; k++) {
				fprintf(stdout, "%s %s\n",mapping.c_str(),interconnects[k].c_str());
				DataflowAnalysis(("data/pe_array/"+interconnects[k]).c_str(),("data/statement/"+statement).c_str(),("data/mapping/"+mapping).c_str());
			}
		}
	}
	
	experiment_file.close();
	return 0;
}

int main(int argc, char * argv[])
{
    printf("Hello world\n");
	ctx = isl_ctx_alloc();
	p = isl_printer_to_file(ctx, stdout);
	p = isl_printer_set_output_format(p, ISL_FORMAT_ISL);
	string experiment_kernel;
	cin>>experiment_kernel;
	experiment(experiment_kernel);
	isl_printer_free(p);
	isl_ctx_free(ctx);
	return 0;
}
