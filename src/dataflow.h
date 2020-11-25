#pragma once
#include "stt.h"
#include "pe_array.h"
#include "statement.h"
#include "mapping.h"

class Dataflow
{
	Statement st;
	PEArray pe;
	Mapping mp;

	int convert_upwqp_to_int(isl_union_pw_qpolynomial *upwqp);
public:
	Dataflow(Statement st_, PEArray pe_, Mapping mp_);
	Dataflow(const Dataflow &df);
	isl_union_map *GetSpaceMap();
	isl_union_map *GetTimeMap();
	isl_union_map *GetSpaceTimeMap();
	isl_union_set *GetDomain();
	int GetDomainSize();
	isl_union_map *GetAccess(string tensor_name,
		AccessType type);
	isl_union_set *GetSpaceDomain();
	isl_union_set *GetTimeDomain();
	isl_union_set *GetSpaceTimeDomain();
	isl_union_map *MapTimeToPrev(unsigned distance, bool is_range);
	isl_union_map *MapSpaceToNeighbor(unsigned distance, bool is_range);
	isl_union_map * MapSpaceTimeToNeighbor(unsigned space_distance=1, 
		bool space_is_range=true, unsigned time_distance = 1, 
		bool time_is_range = true, bool include_self = false);
	isl_union_map * MapSpaceTimeToAccess(string tensor_name, AccessType type);
	int GetPENum();
	int GetTotalTime();
	// following are functions that perform dataflow analysis
	int GetUniqueVolume(string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	int GetTotalVolume(string tensor_name, AccessType type);
	double GetReuseFactor(string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	double GetTemporalReuseVolume(string tensor_name, AccessType type);
	double GetSpatialReuseVolume(string tensor_name, AccessType type);
	int GetMacNum(int mac_per_instance = 1);
	int GetMacNumPerPE(int mac_per_instance = 1);
	int GetActivePENum();
	double GetAverageActivePENum();
	int GetIngressDelay(isl_union_map* space_time_to_neighbor);
	int GetEgressDelay(isl_union_map* space_time_to_neighbor);
	int GetComputationDelay();
	int GetDelay(isl_union_map* space_time_to_neighbor);
	int GetL1Read(string tensor_name, AccessType type);
	int GetL1Write(string tensor_name, AccessType type);
	int GetL2Read(string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	int GetL2Write(string tensor_name, AccessType type,
		isl_union_map* space_time_to_neighbor);
	int GetEnergy(isl_union_map* space_time_to_neighbor);
};
