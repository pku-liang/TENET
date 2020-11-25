2 1
{S[k,c,ox,oy,rx,ry]:0<=k<32 and 0<=c<192 and 0<=ox<28 and 0<=oy<28 and 0<=rx<1 and 0<=ry<1}
{S[k,c,ox,oy,rx,ry]->I[c,ox+rx,oy+ry]}
{S[k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[k,c,ox,oy,rx,ry]->O[k,ox,oy]}