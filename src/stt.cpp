#include"stt.h"
#include "pe_array.h"

using namespace std;

using namespace TENET;

ISL_Context::ISL_Context(FILE* file) :
    _ctx(isl_ctx_alloc()),
    _p(isl_printer_set_output_format(
        isl_printer_to_file(_ctx.get(), file),
        ISL_FORMAT_ISL)
      ),
    _file(file)
{}

ISL_Context::ISL_Context(ISL_Context &&other):
  _ctx(move(other._ctx)),
  _p(other._p),
  _file(other._file)
{
  other._p = nullptr;
}

ISL_Context&
ISL_Context::operator=(ISL_Context &&other)
{
  if (&other != this){
    _ctx = move(other._ctx);
    _p = other._p;
    other._p = nullptr;
    _file = other._file;
  }
  return *this;
}

ISL_Context::~ISL_Context()
{
  isl_printer_free(_p);
}
