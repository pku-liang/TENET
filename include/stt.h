#pragma once


#include<algorithm>
#include<fstream>
#include<map>
#include<memory>
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<vector>

#include<barvinok/barvinok.h>
#include<isl/val.h>

#include "util.h"

namespace TENET
{

const unsigned MAX_STR_LEN{1000};
const unsigned BIT_PER_ITEM{16};
// #define DEBUG
// these multipliers are energy cost per access
// which comes from maestro
const double l2_multiplier{18.61};
const double l1_multiplier{1.68};

class ISL_Context
{
public:
    ISL_Context(FILE*);
    ISL_Context(const ISL_Context&)=delete;
    ISL_Context& operator=(const ISL_Context&)=delete;
    ISL_Context(ISL_Context&&);
    ISL_Context& operator=(ISL_Context&&);
    ~ISL_Context();

    isl_ctx* ctx() const noexcept
    {return _ctx.get();}
    FILE* file() const noexcept
    {return _file;}
    // isl_printer* printer() const noexcept
    // {return _p;}

    template<class Fn, class ...Args>
    void printer(Fn fn, Args&& ...args)
    {
        _p = fn(_p, std::forward<Args>(args)...);
    }

    template<class ...Args>
    int printf(Args&& ...args)
    {
        return fprintf(_file, std::forward<Args>(args)...);
    }

private:
    FILE* _file;
    isl_ctx_ptr _ctx;
    isl_printer *_p;
}; // class ISL_Context

}
// namespace TENET
