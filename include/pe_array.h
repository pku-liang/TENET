#pragma once
#include "stt.h"

namespace TENET
{

class PEArray
{
public:
	PEArray(std::shared_ptr<ISL_Context> context);
	PEArray(
		std::shared_ptr<ISL_Context> context,
		const char* pe_domain_str,
		const char* interconnect_str,
		unsigned l1size,
		unsigned l2size,
		unsigned bandwidth,
		unsigned avg_latency
	);

	bool Load(const char *filename);

	isl_union_set *GetDomain() const
	{return isl_union_set_copy(_domain.get());}

	isl_union_map *GetInterconnect() const noexcept
	{return isl_union_map_copy(_interconnect.get());}

	unsigned GetL1Size() const noexcept
	{return _l1size;}

	unsigned GetL2Size() const noexcept
	{return _l2size;}

	unsigned GetBandwidth() const noexcept
	{return _bandwidth;}
	unsigned GetAvgLatency() const noexcept
	{return _avg_latency;}

	void PrintInfo() const;

	PEArray copy() const;

private:
	isl_union_set_ptr _domain;
	isl_union_map_ptr _interconnect;
	unsigned _l1size{1};
	unsigned _l2size{1};
	unsigned _bandwidth{1};
	unsigned _avg_latency{1};

	std::shared_ptr<ISL_Context> _context;
}; // class PEArray

} // namespace TENET