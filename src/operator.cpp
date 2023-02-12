#include"dataflow.h"

PEArray Load2DSquarePEArray(int height, int width)
{
	char *s = new char[MAX_STR_LEN];
	sprintf(s, "{PE[i,j]:0<=i<%d and 0<=j < %d}", height, width);
	PEArray pe(s, "{PE[i,j]->PE[i+1,j];PE[i,j]->PE[i,j+1]}", 256, 4096, 64);
	delete[] s;
	return pe;
}

PEArray Load1DPEArray(int size)
{
	char *s = new char[MAX_STR_LEN];
	sprintf(s, "{PE[i]:0<=i<%d}", size);
	PEArray pe(s, "{PE[i]->PE[i+1]}", 256, 4096, 64);
	delete[] s;
	return pe;
}

/*
* This statement create a conv2d operator that have input size c*x*y
* and filter size k*c*rx*ry. stride is the step of filter moves on input.
* We don't consider padding currently. The output is of size k*ox*oy.
*/
Statement LoadConv2d(int k,int c, int x, int y, int rx, int ry, int stride)
{
	int padding = 0;
	int ox = (x - rx + 2 * padding) / stride + 1;
	int oy = (y - ry + 2 * padding) / stride + 1;
	char * domain_str = new char[MAX_STR_LEN];
	sprintf(domain_str, "{S[k,c,ox,oy,rx,ry]:0<=k<%d and 0<=c<%d and 0<=ox<%d "
		"and 0<=oy<%d and 0<=rx<%d and 0<=ry<%d}", k, c, ox, oy, rx, ry);
	Statement s(domain_str);
	Access o_write("O", "{S[k,c,ox,oy,rx,ry]->O[k,ox,oy]}",true);
	Access o_read("O", "{S[k,c,ox,oy,rx,ry]->O[k,ox,oy]}", false);
	char * access_str = new char[MAX_STR_LEN];
	sprintf(access_str, "{S[k,c,ox,oy,rx,ry]->I[c,%d*ox+rx,%d*oy+ry]}", stride, stride);
	Access i_read("I", access_str, false);
	Access w_read("W", "{S[k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}", false);
	s.AddAccess(o_write);
	s.AddAccess(o_read);
	s.AddAccess(i_read);
	s.AddAccess(w_read);
	delete[] domain_str;
	delete[] access_str;
	return s;
}

/*
* This statement create a gemm operator that have 
* C[n][m] as output, A[n][c] and B[c][m] as input.
*/
Statement LoadGemm(int n, int c, int m)
{
	char * domain_str = new char[MAX_STR_LEN];
	sprintf(domain_str, "{S[i,j,k]:0<=i<%d and 0<=j<%d and 0<=k<%d}", n, c, m);
	Statement s(domain_str);
	Access c_write("C", "{S[i,j,k]->C[i,k]}", true);
	Access c_read("C", "{S[i,j,k]->C[i,k]}", false);
	Access a_read("A", "{S[i,j,k]->A[i,j]}", false);
	Access b_read("B", "{S[i,j,k]->B[j,k]}", false);
	s.AddAccess(c_read);
	s.AddAccess(c_write);
	s.AddAccess(a_read);
	s.AddAccess(b_read);
	delete[] domain_str;
	return s;
}

/*
* Load nlr dataflow of conv2d
*/
Dataflow LoadNlrConv2d(PEArray& pe_, Statement& st_)
{
	Mapping mp("{S[k,c,ox,oy,rx,ry]->PE[c]}", "{S[k,c,ox,oy,rx,ry]->T[k,oy,ox]}");
	return Dataflow(st_, pe_, mp);
}



