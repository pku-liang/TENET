#pragma once
#include "dataflow.h"

PEArray Load2DSquarePEArray(int height, int width);
PEArray Load1DPEArray(int size);
Statement LoadConv2d(int k, int c, int x, int y, int rx, int ry, int stride);
Statement LoadGemm(int n, int c, int m);
Dataflow LoadNlrConv2d(PEArray& pe_, Statement& st_);
