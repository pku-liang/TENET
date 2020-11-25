#pragma once
#include "stt.h"

class PEArray
{
	isl_union_set *domain;
	isl_union_map *interconnect;
	unsigned l1size, l2size, bandwidth,avg_latency;
public:
	PEArray();
	PEArray(const char* pe_domain_str, const char* interconnect_str,
		unsigned l1size_, unsigned l2size_, unsigned bandwidth_, 
		unsigned avg_latency);
	PEArray(const PEArray & pe_array);
	const PEArray& operator=(const PEArray & pe_array);
	~PEArray();
	bool Load(const char *filename);
	isl_union_set *GetDomain();
	isl_union_map *GetInterconnect();
	unsigned GetL1Size();
	unsigned GetL2Size();
	unsigned GetBandwidth();
	unsigned GetAvgLatency();
	void PrintInfo();
};
