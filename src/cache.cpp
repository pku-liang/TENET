#include "cache.h"

Cache::Cache()
{
    _dat2mem = NULL;
}

Cache::Cache(string name, int n_blk, int n_row, const char* array_str,const char* dat2mem_map_str)
{
    _dat_name = name;

    _n_blk = n_blk;
    _n_row = n_row;
    
    _array = isl_union_set_read_from_str(ctx, array_str);
	_dat2mem = isl_union_map_read_from_str(ctx, dat2mem_map_str);
}

Cache::Cache(const Cache &cache)
{
    _dat_name = cache._dat_name;

	_n_blk = cache._n_blk;
    _n_row = cache._n_row;
    
    _array = cache._array;
    _dat2mem = cache._dat2mem;
}

const Cache& Cache::operator=(const Cache& cache)
{
    _dat_name = cache._dat_name;
	
    _n_blk = cache._n_blk;
    _n_row = cache._n_row;
    
    if (_array != NULL)
        isl_union_set_free(_array);
    if (_dat2mem != NULL)
        isl_union_map_free(_dat2mem);
	
    _array = cache._array;
    _dat2mem = cache._dat2mem;
}

Cache::~Cache()
{
    if (_array != NULL)
        isl_union_set_free(_array);
	if (_dat2mem != NULL)
		isl_union_map_free(_dat2mem);
}
bool Cache::Load(const char* filename, std::map<string, Cache*>& caches)
{
	ifstream input(filename);
	if (!input.is_open())
		return false;
	int num_dat;
	input >> num_dat;
    string endl;
    getline(input,endl);
    for(int i = 0; i < num_dat; i++) {
        string dat_name, dat2mem_map_str, array_str, tmp;
        int n_blk, n_row;
        input >> dat_name >> n_blk >> n_row;
        getline(input, endl);
        // printf("%s %d %d\n", dat_name.c_str(), n_blk, n_row);
        getline(input, array_str);
        // printf("array string: %s\n", array_str.c_str());
        getline(input, dat2mem_map_str);
        // printf("data layout: %s\n", dat2mem_map_str.c_str());
        caches[dat_name] = new Cache(dat_name, n_blk, n_row, array_str.c_str(), dat2mem_map_str.c_str());
    }
	input.close();
    return true;
}

isl_union_map* Cache::Get_dat2mem() { return isl_union_map_copy(_dat2mem); }

void Cache::PrintInfo() {
    fprintf(stdout, "data name: %s n_row=%d n_block=%d\n", _dat_name.c_str(), _n_row, _n_blk);
	fprintf(stdout, "array:");
    p = isl_printer_print_union_set(p, _array);
	fprintf(stdout, "\ndata_layout: ");
	p = isl_printer_print_union_map(p, _dat2mem);
}