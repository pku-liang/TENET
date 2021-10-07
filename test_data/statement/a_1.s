2 1
{S[n,k,c,ox,oy,rx,ry]:0<=n<4 and 0<=k<100 and 0<=c<4 and 0<=ox<40 and 0<=oy<40 and 0<=rx<5 and 0<=ry<5}
{S[n,k,c,ox,oy,rx,ry]->I[n,c,ox*4+rx,oy*4+ry]}
{S[n,k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[n,k,c,ox,oy,rx,ry]->O[n,k,ox,oy]}