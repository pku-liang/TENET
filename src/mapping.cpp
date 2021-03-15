#include"mapping.h"

Mapping::Mapping()
{
	space_map = NULL;
	time_map = NULL;
}

Mapping::Mapping(const char* space_map_str, const char* time_map_str)
{
	space_map = isl_union_map_read_from_str(ctx, space_map_str);
	time_map = isl_union_map_read_from_str(ctx, time_map_str);
}

Mapping::Mapping(const Mapping &mp)
{
	time_map = isl_union_map_copy(mp.time_map);
	space_map = isl_union_map_copy(mp.space_map);
}

const Mapping& Mapping::operator=(const Mapping& mp)
{
	if (time_map != NULL)
		isl_union_map_free(time_map);
	if (space_map != NULL)
		isl_union_map_free(space_map);
	time_map = isl_union_map_copy(mp.time_map);
	space_map = isl_union_map_copy(mp.space_map);
    return *this;
}

Mapping::~Mapping()
{
	if (time_map != NULL)
		isl_union_map_free(time_map);
	if (space_map != NULL)
		isl_union_map_free(space_map);
}

bool Mapping::Load(const char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	string space_map_str, time_map_str;
	getline(input, space_map_str);
	getline(input, time_map_str);
	input.close();
	if (time_map != NULL)
		isl_union_map_free(time_map);
	if (space_map != NULL)
		isl_union_map_free(space_map);
	space_map = isl_union_map_read_from_str(ctx, space_map_str.c_str());
	time_map = isl_union_map_read_from_str(ctx, time_map_str.c_str());
	return true;
}

isl_union_map *Mapping::GetSpaceMap()
{
	return isl_union_map_copy(space_map);
}

isl_union_map *Mapping::GetTimeMap()
{
	return isl_union_map_copy(time_map);
}

isl_union_map *Mapping::GetSpaceTimeMap()
{
	return isl_union_map_range_product(isl_union_map_copy(space_map),
		isl_union_map_copy(time_map));
}

void Mapping::PrintInfo()
{
	p = isl_printer_print_str(p, "SpaceMap:\n");
	p = isl_printer_print_union_map(p, space_map);
	p = isl_printer_print_str(p, "\nTimeMap:\n");
	p = isl_printer_print_union_map(p, time_map);
	p = isl_printer_end_line(p);
}
