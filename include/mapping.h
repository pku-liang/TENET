#pragma once
#include "stt.h"

namespace TENET
{

class Mapping
{
public:
	Mapping(std::shared_ptr<ISL_Context> context);
	Mapping(
		std::shared_ptr<ISL_Context> context,
		const char* space_map_str,
		const char* time_map_str
	);
	bool Load(const char* filename);

	isl_union_map *GetSpaceMap() const noexcept
	{return isl_union_map_copy(_space_map.get());}
	isl_union_map *GetTimeMap() const noexcept
	{return isl_union_map_copy(_time_map.get());}
	isl_union_map *GetSpaceTimeMap() const noexcept
	{
	    return isl_union_map_range_product(
			isl_union_map_copy(_space_map.get()),
			isl_union_map_copy(_time_map.get())
        );
    }

	void PrintInfo() const;

	Mapping copy() const;
private:
	std::shared_ptr<ISL_Context> _context;
	isl_union_map_ptr _space_map;
	isl_union_map_ptr _time_map;

}; // class Mapping

} // namespace TENET