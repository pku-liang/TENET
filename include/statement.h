#pragma once
#include"stt.h"

namespace TENET
{

enum class AccessType
{
	READ,
	WRITE,
	READ_OR_WRITE
}; // enum class Access Type

class Access
{
public:
	friend class Statement;

	Access(std::shared_ptr<ISL_Context> context);
	Access(
		std::shared_ptr<ISL_Context> context,
		std::string tensor_name,
		const char* access_str,
		bool is_write_
	);
	isl_union_map *GetAccess() const noexcept
	{return isl_union_map_copy(_access.get());}
	void PrintInfo() const;

	Access copy() const;
private:
	std::string _tensor_name;
	isl_union_map_ptr _access;
	bool _is_write{false};

	std::shared_ptr<ISL_Context> _context;

}; // class Access


class Statement
{
public:
	Statement(std::shared_ptr<ISL_Context> context);
	Statement(
		std::shared_ptr<ISL_Context> context,
		const char* statement_domain_str
	);

	void AddAccess(Access &&ac);
	bool Load(const char* filename);

	isl_union_set *GetDomain() const
	{return isl_union_set_copy(_domain.get());}

	isl_union_map *GetAccess(std::string tensor_name, AccessType type) const;

	void PrintInfo() const;

	std::pair<std::vector<std::string>, std::vector<std::string>>
	GetTensorList() const;

	Statement copy() const;
private:
	isl_union_set_ptr _domain;
	std::vector<Access> _read;
	std::vector<Access> _write;
	std::shared_ptr<ISL_Context> _context;
};

} // namespace TENET