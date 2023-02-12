#include"dataflow.h"

#define USE_GEMM 1
#define USE_CONV 0

void DataflowAnalysis(const char* _pe_file, const char* _statement_file,
	const char* _mapping_file, const char* _cache_file)
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
	std::map<string, Cache*> caches;
	if (!Cache::Load(_cache_file, caches))
	{
		fprintf(stderr, "Load Cache failed\n");
	}
	// pe.PrintInfo();
	// st.PrintInfo();
	// mp.PrintInfo();
	
	// for (auto cache: caches) {
	// 	cache.second->PrintInfo();
	// }

	Dataflow df(st, pe, mp, caches);
	vector<string> input, output;
	st.GetTensorList(input, output);
	isl_union_map *space_time_to_neighbor = df.MapSpaceTimeToNeighbor();
	isl_union_map *temporal_reuse_st2neighbor = df.MapSpaceTimeToNeighbor(0, true, 1, true, false);
	isl_union_map *spatial_reuse_st2neighbor = df.MapSpaceTimeToNeighbor(1, false, 1, true, false);
#ifdef DEBUG
	fprintf(stdout, "Space Domain:\n");
	p = isl_printer_print_union_set(p, df.GetSpaceDomain());
	fprintf(stdout, "Reuse Map:\n");
	p = isl_printer_print_union_map(p, space_time_to_neighbor);
	p = isl_printer_end_line(p);
#endif
	for (auto iter = input.begin(); iter != input.end(); iter++)
	{
		// double reuse_factor = df.GetReuseFactor(*iter, READ, isl_union_map_copy(space_time_to_neighbor));
		int pad_reuse_volume = df.GetUniqueVolume(iter->c_str(), READ, isl_union_map_copy(space_time_to_neighbor));
		int totalvolume = df.GetTotalVolume(iter->c_str(), READ);
		int temporal_reuse_volume = totalvolume - df.GetUniqueVolume(iter->c_str(), READ, isl_union_map_copy(temporal_reuse_st2neighbor));
		int spatial_reuse_volume = totalvolume - df.GetUniqueVolume(iter->c_str(), READ, isl_union_map_copy(spatial_reuse_st2neighbor));
		// int dram_fetch_volume = 0;
		int dram_fetch_volume = df.CacheMiss(iter->c_str(), READ) ;
		fprintf(stdout, "Input Tensor: %s\n temporal reuse %d, spatial reuse %d, pad reuse %d, dram fetch volume %d\n", iter->c_str(), temporal_reuse_volume, spatial_reuse_volume, pad_reuse_volume, dram_fetch_volume);
	}
	for (auto iter = output.begin(); iter != output.end(); iter++)
	{
		int pad_reuse_volume = df.GetUniqueVolume(iter->c_str(), WRITE, isl_union_map_copy(space_time_to_neighbor));
		int totalvolume = df.GetTotalVolume(iter->c_str(), WRITE);
		int temporal_reuse_volume = totalvolume - df.GetUniqueVolume(iter->c_str(), WRITE, isl_union_map_copy(temporal_reuse_st2neighbor));
		int spatial_reuse_volume = totalvolume - df.GetUniqueVolume(iter->c_str(), WRITE, isl_union_map_copy(spatial_reuse_st2neighbor));
		int dram_fetch_volume = df.CacheMiss(iter->c_str(), WRITE);
		fprintf(stdout, "Output Tensor: %s\n temporal reuse %d, spatial reuse %d, pad reuse %d, dram fetch volume %d\n", iter->c_str(), temporal_reuse_volume, spatial_reuse_volume, pad_reuse_volume, dram_fetch_volume);
	}
	
	int ingress_delay = df.GetIngressDelay(isl_union_map_copy(space_time_to_neighbor));
	int egress_delay = df.GetEgressDelay(isl_union_map_copy(space_time_to_neighbor));
	int computation_delay = df.GetComputationDelay();
	fprintf(stdout, "Delay: In: %d; Out: %d; Com: %d\n", ingress_delay, egress_delay, computation_delay);

	// for (auto iter = input.begin(); iter != input.end(); iter++)
	// {
	// 	printf("Data %s cache miss: \n", (*iter).c_str());
	// 	df.CacheMiss(*iter, READ);
	// }

	isl_union_map_free(space_time_to_neighbor);
}


int main(int argc, char * argv[])
{
	ctx = isl_ctx_alloc();
	p = isl_printer_to_file(ctx, stdout);
	p = isl_printer_set_output_format(p, ISL_FORMAT_ISL);
	// const char* pe_file = "data/pe_array/pe_16_64.txt";
	// const char* statement_file = "data/statement/conv6_googlenet.txt";
	// const char* mapping_file = "data/mapping/conv2d_ws_kcp.txt";
#if USE_GEMM
	const char* pe_file = "cache_experi/gemm/pe_array.txt";
	const char* statement_file = "cache_experi/gemm/gemm.txt";
	const char* mapping_file = "cache_experi/gemm/systolic_os.txt";
	const char* cache_file = "cache_experi/gemm/cache.txt";
#endif
#if USE_CONV
	const char* pe_file = "cache_experi/conv2d/pe_array.txt";
	const char* statement_file = "cache_experi/conv2d/conv2d.txt";
	const char* mapping_file = "cache_experi/conv2d/conv2d_eyeriss2d.txt";
	const char* cache_file = "cache_experi/conv2d/cache.txt";
#endif
	DataflowAnalysis(pe_file, statement_file, mapping_file, cache_file);
	isl_printer_free(p);
	isl_ctx_free(ctx);
	return 0;
}