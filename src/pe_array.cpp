#include "pe_array.h"

using namespace std;
using namespace TENET;

PEArray::PEArray(shared_ptr<ISL_Context> context):
	_domain(nullptr),
	_interconnect(nullptr),
	_context(context)
{}

PEArray::PEArray(
	shared_ptr<ISL_Context> context,
	const char* pe_domain_str,
	const char*interconnect_str,
	unsigned l1size,
	unsigned l2size,
	unsigned bandwidth,
	unsigned avg_latency):
	_domain(isl_union_set_read_from_str(context->ctx(), pe_domain_str)),
	_interconnect(
		isl_union_map_intersect_range(
			isl_union_map_intersect_domain(
				isl_union_map_read_from_str(context->ctx(), interconnect_str),
				isl_union_set_copy(_domain.get())),
			isl_union_set_copy(_domain.get())
		)
	),
	_l1size(l1size),
	_l2size(l2size),
	_bandwidth(bandwidth),
	_avg_latency(avg_latency),
	_context(context)
{}

bool
PEArray::Load(const char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	string domain_str, interconnect_str;
	getline(input, domain_str);
	getline(input, interconnect_str);

	_domain.reset(
		isl_union_set_read_from_str(_context->ctx(), domain_str.c_str())
	);
	_interconnect.reset(
		isl_union_map_read_from_str(_context->ctx(), interconnect_str.c_str())
	);
	input >> _l1size >> _l2size >> _bandwidth >> _avg_latency;
	input.close();
	return true;
}

void
PEArray::PrintInfo() const
{
	_context->printf("pe_domain: ");
	_context->printer(isl_printer_print_union_set, _domain.get());
	_context->printf("\ninterconnection: ");
	_context->printer(isl_printer_print_union_map, _interconnect.get());
	_context->printf("\nL1Size: %u\nL2Size: %u\nBandwidth: %u\n", _l1size, _l2size, _bandwidth);
}

PEArray
PEArray::copy() const
{
	PEArray result{_context};
	result._domain.reset(
		isl_union_set_copy(_domain.get())
	);
	result._interconnect.reset(
		isl_union_map_copy(_interconnect.get())
	);
	result._l1size = _l1size;
	result._l2size = _l2size;
	result._bandwidth = _bandwidth;
	result._avg_latency = _avg_latency;
	return result;
}
