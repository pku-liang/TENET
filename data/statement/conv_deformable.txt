2 1
{S[k,c,ox,oy,rx,ry]:0<=k<64 and 0<=c<3 and 0<=ox<222 and 0<=oy<222 and 0<=rx<3 and 0<=ry<3}
{S[k,c,ox,oy,rx,ry]->I[c,ox+2*rx-ry,oy+rx+2*ry-2]}
{S[k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[k,c,ox,oy,rx,ry]->O[k,ox,oy]}