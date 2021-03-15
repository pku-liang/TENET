2 1
{S[n,k,c,ox,oy,rx,ry]:0<=n<4 and 0<=k<256 and 0<=c<48 and 0<=ox<27 and 0<=oy<27 and 0<=rx<5 and 0<=ry<5}
{S[n,k,c,ox,oy,rx,ry]->I[n,c,ox+rx,oy+ry]}
{S[n,k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[n,k,c,ox,oy,rx,ry]->O[n,k,ox,oy]}