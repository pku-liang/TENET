#include"statement.h"

using namespace std;
using namespace TENET;

Access::Access(shared_ptr<ISL_Context> context)
	:_context(context)
{}
Access::Access(
	shared_ptr<ISL_Context> context,
	string tensor_name,
	const char* access_str,
	bool is_write):
	_tensor_name(tensor_name),
	_access(isl_union_map_read_from_str(context->ctx(), access_str)),
	_is_write(is_write),
	_context(context)
{}

void
Access::PrintInfo() const
{
	_context->printf("Tensor Name: %s\n", _tensor_name.c_str());
	const char* access_type = _is_write ? "write" : "read";
	_context->printf("Access Type: %s\n", access_type);
	_context->printf("Access: ");
	_context->printer(isl_printer_print_union_map, _access.get());
	_context->printf("\n\n");
}

Access
Access::copy() const
{
	Access result{_context};
	result._tensor_name = _tensor_name;
	result._access.reset(
		isl_union_map_copy(_access.get())
	);
	result._is_write = _is_write;
	return result;
}


Statement::Statement(shared_ptr<ISL_Context> context):
	_context(context)
{}

Statement::Statement(
	shared_ptr<ISL_Context> context,
	const char* statement_domain_str):
	_domain(isl_union_set_read_from_str(context->ctx(), statement_domain_str)),
	_context(context)
{}

void
Statement::AddAccess(Access &&ac)
{
	if (ac._is_write == true)
	{
		_write.push_back(move(ac));
	}
	else
	{
		_read.push_back(move(ac));
	}
}

bool
Statement::Load(const char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	string domain_str, access_str;
	int read_num, write_num;
	input >> read_num >> write_num;
	getline(input, domain_str);
	getline(input, domain_str);

	_domain.reset(
		isl_union_set_read_from_str(_context->ctx(), domain_str.c_str())
	);
	_read.clear();
	_write.clear();

	for (int i = 0; i < read_num; i++)
	{
		getline(input, access_str);
		size_t pos = access_str.rfind("->");
		pos = pos + 2;
		size_t len = 0;
		while (access_str[pos + len] != '[')
			len++;
		string tensor_name = access_str.substr(pos, len);
		this->AddAccess(
			Access{_context, tensor_name, access_str.c_str(), false}
		);
	}

	for (int i = 0; i < write_num; i++)
	{
		getline(input, access_str);
		size_t pos = access_str.rfind("->");
		pos = pos + 2;
		size_t len = 0;
		while (access_str[pos + len] != '[')
			len++;
		string tensor_name = access_str.substr(pos, len);
		this->AddAccess(
			Access{_context, tensor_name, access_str.c_str(), true}
		);
	}
	input.close();
	return true;
}

isl_union_map*
Statement::GetAccess(
	string tensor_name,
	AccessType type) const
{
	isl_union_map *ret = NULL;
	if (type == AccessType::READ or type == AccessType::READ_OR_WRITE)
	{
		for (auto& ac : _read)
			if (tensor_name == "" || tensor_name == ac._tensor_name ) // the same tensor
			{
				if (ret == NULL)
					ret = ac.GetAccess();
				else
					ret = isl_union_map_union(ret, ac.GetAccess());
			}
	}
	if (type == AccessType::WRITE or type == AccessType::READ_OR_WRITE)
	{
		for (auto &ac : _write)
			if (tensor_name == "" || tensor_name == ac._tensor_name) // the same tensor
			{
				if (ret == NULL)
					ret = ac.GetAccess();
				else
					ret = isl_union_map_union(ret, ac.GetAccess());
			}
	}
	ret = isl_union_map_intersect_domain(ret, this->GetDomain());
	return ret;
}

void
Statement::PrintInfo() const
{
	_context->printf("Statement Domain: ");
	_context->printer(isl_printer_print_union_set, _domain.get());
	_context->printf("\nRead Access: \n");
	for (auto &it : _read)
		it.PrintInfo();
	_context->printf("\nWrite Access: \n");
	for (auto &it : _write)
		it.PrintInfo();
}

pair<vector<string>, vector<string>>
Statement::GetTensorList() const
{
	vector<string> input;
	transform(_read.begin(), _read.end(), back_inserter(input),
		[](auto& iter) { return iter._tensor_name;});
	sort(input.begin(), input.end());
	input.erase(unique(input.begin(), input.end()), input.end());

	vector<string> output;
	transform(_write.begin(), _write.end(), back_inserter(output),
		[](auto& iter) { return iter._tensor_name;});
	sort(output.begin(), output.end());
	output.erase(unique(output.begin(), output.end()), output.end());

	return make_pair(input, output);
}

Statement
Statement::copy() const
{
	Statement result{_context};
	result._domain.reset(isl_union_set_copy(_domain.get()));
	return result;
}
