#include"mapping.h"

using namespace std;
using namespace TENET;

Mapping::Mapping(shared_ptr<ISL_Context> context):
	_context(context)
{}

Mapping::Mapping(
	shared_ptr<ISL_Context> context,
	const char* space_map_str,
	const char* time_map_str):
	_space_map(isl_union_map_read_from_str(context->ctx(), space_map_str)),
	_time_map(isl_union_map_read_from_str(context->ctx(), time_map_str)),
	_context(context)
{}

bool
Mapping::Load(const char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	string space_map_str, time_map_str;
	getline(input, space_map_str);
	getline(input, time_map_str);
	input.close();

	_space_map.reset(
		isl_union_map_read_from_str(_context->ctx(), space_map_str.c_str())
	);
	_time_map.reset(
		isl_union_map_read_from_str(_context->ctx(), time_map_str.c_str())
	);
	return true;
}


void
Mapping::PrintInfo() const
{
	_context->printer(isl_printer_print_str, "SpaceMap:\n");
	_context->printer(isl_printer_print_union_map, _space_map.get());
	_context->printer(isl_printer_print_str, "\nTimeMap:\n");
	_context->printer(isl_printer_print_union_map, _time_map.get());
	_context->printer(isl_printer_end_line);
}

Mapping
Mapping::copy() const
{
	Mapping result(_context);
	result._space_map.reset(
		isl_union_map_copy(_space_map.get())
	);
	result._time_map.reset(
		isl_union_map_copy(_time_map.get())
	);
	return result;
}
