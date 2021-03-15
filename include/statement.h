#pragma once
#include"stt.h"

enum AccessType
{
	READ,
	WRITE,
	READ_OR_WRITE
};

class Access
{
	string tensor_name;
	isl_union_map *access;
	bool is_write;
public:
	friend class Statement;
	Access(string tensor_name_, const char* access_str, bool is_write_);
	Access(const Access & ac);
	const Access &operator=(const Access & ac);
	isl_union_map *GetAccess();
	void PrintInfo();
	~Access();
};


class Statement
{
	isl_union_set *domain;
	std::vector<Access> read;
	std::vector<Access> write;
public:
	Statement();
	Statement(const char* statement_domain_str);
	Statement(const Statement & st);
	const Statement &operator=(const Statement &st);
	~Statement();
	void AddAccess(Access ac);
	bool Load(const char* filename);
	isl_union_set *GetDomain();
	isl_union_map *GetAccess(string tensor_name,
		AccessType type);
	void PrintInfo();
	void GetTensorList(vector<string>& input, vector<string>& output);
};