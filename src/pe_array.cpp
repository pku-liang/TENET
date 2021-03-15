#include "pe_array.h"

PEArray::PEArray()
{
	domain = NULL;
	interconnect = NULL;
	l1size = l2size = bandwidth = 1;
}

PEArray::PEArray(const char* pe_domain_str, const char*interconnect_str,
	unsigned l1size_, unsigned l2size_, unsigned bandwidth_, unsigned avg_latency_)
{
	domain = isl_union_set_read_from_str(ctx, pe_domain_str);
	interconnect = isl_union_map_read_from_str(ctx, interconnect_str);
	interconnect = isl_union_map_intersect_domain(interconnect, isl_union_set_copy(domain));
	interconnect = isl_union_map_intersect_range(interconnect, isl_union_set_copy(domain));
	l1size = l1size_;
	l2size = l2size_;
	bandwidth = bandwidth_;
	avg_latency = avg_latency_;
}

PEArray::PEArray(const PEArray & pe_array)
{
	domain = isl_union_set_copy(pe_array.domain);
	interconnect = isl_union_map_copy(pe_array.interconnect);
	l1size = pe_array.l1size;
	l2size = pe_array.l2size;
	bandwidth = pe_array.bandwidth;
	avg_latency = pe_array.avg_latency;
}

const PEArray& PEArray::operator=(const PEArray & pe_array)
{
	if (domain != NULL)
		isl_union_set_free(domain);
	if (interconnect != NULL)
		isl_union_map_free(interconnect);
	domain = isl_union_set_copy(pe_array.domain);
	interconnect = isl_union_map_copy(pe_array.interconnect);
	l1size = pe_array.l1size;
	l2size = pe_array.l2size;
	bandwidth = pe_array.bandwidth;
	avg_latency = pe_array.avg_latency;
	return *this;
}

PEArray::~PEArray()
{
	if (domain != NULL)
		isl_union_set_free(domain);
	if (interconnect != NULL)
		isl_union_map_free(interconnect);
}

bool PEArray::Load(const char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	string domain_str, interconnect_str;
	getline(input, domain_str);
	getline(input, interconnect_str);
	if (domain != NULL)
		isl_union_set_free(domain);
	if (interconnect != NULL)
		isl_union_map_free(interconnect);
	domain = isl_union_set_read_from_str(ctx, domain_str.c_str());
	interconnect = isl_union_map_read_from_str(ctx, interconnect_str.c_str());
	input >> l1size >> l2size >> bandwidth >> avg_latency;
	input.close();
	return true;
}

isl_union_set *PEArray::GetDomain()
{
	return isl_union_set_copy(domain);
}

isl_union_map *PEArray::GetInterconnect()
{
	return isl_union_map_copy(interconnect);
}

unsigned PEArray::GetL1Size()
{
	return l1size;
}

unsigned PEArray::GetL2Size()
{
	return l2size;
}

unsigned PEArray::GetBandwidth()
{
	return bandwidth;
}

unsigned PEArray::GetAvgLatency()
{
	return avg_latency;
}

void PEArray::PrintInfo()
{
	fprintf(stdout, "pe_domain: ");
	p = isl_printer_print_union_set(p, domain);
	fprintf(stdout, "\ninterconnection: ");
	p = isl_printer_print_union_map(p, interconnect);
	fprintf(stdout, "\nL1Size: %u\nL2Size: %u\nBandwidth: %u\n", l1size, l2size, bandwidth);
}
