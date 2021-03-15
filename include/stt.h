#pragma once


#include<barvinok/barvinok.h>
#include<isl/val.h>
#include<vector>
#include<map>
#include<string>
#include<fstream>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<algorithm>

using namespace std;
#define MAX_STR_LEN 1000
#define BIT_PER_ITEM 16
// #define DEBUG
// these multipliers are energy cost per access
// which comes from maestro
const double l2_multiplier = 18.61;
const double l1_multiplier = 1.68; 

extern isl_ctx *ctx;
extern isl_printer *p;
// test functions
/*
int test_PEArray(isl_ctx *ctx, isl_printer *p);
int test_access(isl_ctx *ctx, isl_printer *p);
int test_statement(isl_ctx *ctx, isl_printer *p);
int test_dataflow(isl_ctx *ctx, isl_printer *p);
int test_dataflow_analysis(isl_ctx *ctx, isl_printer *p);
int test_dataload(const char* pe_file, const char* mapping_file, const char* statement_file);
*/