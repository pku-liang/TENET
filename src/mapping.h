#pragma once
#include "stt.h"
class Mapping
{
	isl_union_map *space_map, *time_map;
public:
	Mapping();
	Mapping(const char* space_map_str, const char* time_map_str);
	Mapping(const Mapping &mp);
	const Mapping& operator=(const Mapping& mp);
	~Mapping();
	bool Load(const char* filename);
	isl_union_map *GetSpaceMap();
	isl_union_map *GetTimeMap();
	isl_union_map *GetSpaceTimeMap();
	void PrintInfo();
};