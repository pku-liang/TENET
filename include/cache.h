#pragma once
#include "stt.h"

// S[i,j,k]: 0<=i<8; 0<=j<8; 0<=k<8 
// PE[i,j]: 0<=i<4; 0<=j<4
// S[i,j,k]->T[floor(i/4), floor(k/4), floor(j/4), k%4]
// S[i,j,k]->PE[i%4, j%4]
// S[i,j,k]->A[i,k]      A[i,k]->M_A[i%4, k%4]
// S[i,j,k]->B[k,j]      B[k,j]->M_B[j, k%4]
// S[i,j,k]->C[i,j]      C[i,j]->M_C[i%4, j]

class Cache {
    isl_union_map* _dat2mem;
    std::string _dat_name;
    int _n_row;
    int _n_blk;
public:
    isl_union_set* _array;
    Cache();
    Cache(string name, int n_blk, int n_row, const char* array_str, const char* dat2mem_map_str);
    Cache(const Cache &cache);
    const Cache& operator=(const Cache& cache);
    ~Cache();
    int get_n_row();
    int get_n_blk();
    static bool Load(const char* filename, std::map<string, Cache*>& caches);
    isl_union_map* Get_dat2mem();
    void PrintInfo();
};