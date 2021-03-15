#include"statement.h"

Access::Access(string tensor_name_, const char* access_str, bool is_write_)
{
	tensor_name = tensor_name_;
	access = isl_union_map_read_from_str(ctx, access_str);
	is_write = is_write_;
}

Access::Access(const Access & ac)
{
	tensor_name = ac.tensor_name;
	access = isl_union_map_copy(ac.access);
	is_write = ac.is_write;
}

const Access& Access::operator=(const Access & ac)
{
	tensor_name = ac.tensor_name;
	isl_union_map_free(access);
	access = isl_union_map_copy(ac.access);
	is_write = ac.is_write;
	return *this;
}

isl_union_map * Access::GetAccess()
{
	return isl_union_map_copy(access);
}

void Access::PrintInfo()
{
	fprintf(stdout, "Tensor Name: %s\n", tensor_name.c_str());
	const char* access_type = is_write ? "write" : "read";
	fprintf(stdout, "Access Type: %s\n", access_type);
	fprintf(stdout, "Access: ");
	p = isl_printer_print_union_map(p, access);
	fprintf(stdout, "\n\n");
}

Access::~Access()
{
	isl_union_map_free(access);
}

Statement::Statement()
{
	domain = NULL;
}

Statement::Statement(const char* statement_domain_str)
{
	domain = isl_union_set_read_from_str(ctx, statement_domain_str);
}

Statement::Statement(const Statement & st)
{
	domain = isl_union_set_copy(st.domain);
	read = st.read;
	write = st.write;
}

const Statement &Statement::operator=(const Statement &st)
{
	if (domain != NULL)
		isl_union_set_free(domain);
	domain = isl_union_set_copy(st.domain);
	read = st.read;
	write = st.write;
	return *this;
}

Statement::~Statement()
{
	if (domain != NULL)
		isl_union_set_free(domain);
}

void Statement::AddAccess(Access ac)
{
	if (ac.is_write == true)
	{
		write.push_back(ac);
	}
	else
	{
		read.push_back(ac);
	}
}

bool Statement::Load(const char* filename)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	string domain_str, access_str;
	int read_num, write_num;
	input >> read_num >> write_num;
	getline(input, domain_str);
	getline(input, domain_str);
	if (domain != NULL)
		isl_union_set_free(domain);
	domain = isl_union_set_read_from_str(ctx, domain_str.c_str());
	read.clear();
	write.clear();
	for (int i = 0; i < read_num; i++)
	{
		getline(input, access_str);
		size_t pos = access_str.rfind("->");
		pos = pos + 2;
		size_t len = 0;
		while (access_str[pos + len] != '[')
			len++;
		string tensor_name = access_str.substr(pos, len);
		Access ac(tensor_name, access_str.c_str(), false);
		this->AddAccess(ac);
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
		Access ac(tensor_name, access_str.c_str(), true);
		this->AddAccess(ac);
	}
	input.close();
	return true;
}

isl_union_set * Statement::GetDomain()
{
	return isl_union_set_copy(domain);
}

isl_union_map * Statement::GetAccess(string tensor_name,
	AccessType type)
{
	isl_union_map *ret = NULL;
	if (type == READ or type == READ_OR_WRITE)
	{
		for (auto it = read.begin(); it != read.end(); it++)
			if (tensor_name == "" || tensor_name == it->tensor_name ) // the same tensor
			{
				if (ret == NULL)
					ret = it->GetAccess();
				else
					ret = isl_union_map_union(ret, it->GetAccess());
			}
	}
	if (type == WRITE or type == READ_OR_WRITE)
	{
		for (auto it = write.begin(); it != write.end(); it++)
			if (tensor_name == "" || tensor_name == it->tensor_name) // the same tensor
			{
				if (ret == NULL)
					ret = it->GetAccess();
				else
					ret = isl_union_map_union(ret, it->GetAccess());
			}
	}
	ret = isl_union_map_intersect_domain(ret, this->GetDomain());
	return ret;
}

void Statement::PrintInfo()
{
	fprintf(stdout, "Statement Domain: ");
	p = isl_printer_print_union_set(p, domain);
	fprintf(stdout, "\nRead Access: \n");
	for (auto iter = read.begin(); iter != read.end(); iter++)
		iter->PrintInfo();
	fprintf(stdout, "\nWrite Access: \n");
	for (auto iter = write.begin(); iter != write.end(); iter++)
		iter->PrintInfo();
}

void Statement::GetTensorList(vector<string>& input, vector<string>& output)
{
	input.clear();	
	for (auto iter = read.begin(); iter != read.end(); iter++)
		input.push_back(iter->tensor_name);
	sort(input.begin(), input.end());
	input.erase(unique(input.begin(), input.end()), input.end());

	output.clear();
	for (auto iter = write.begin(); iter != write.end(); iter++)
		output.push_back(iter->tensor_name);
	sort(output.begin(), output.end());
	output.erase(unique(output.begin(), output.end()), output.end());
}