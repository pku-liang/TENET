2 1
{S[n,k,c,ox,oy,rx,ry]:0<=n<4 and 0<=k<384 and 0<=c<192 and 0<=ox<13 and 0<=oy<13 and 0<=rx<3 and 0<=ry<3}
{S[n,k,c,ox,oy,rx,ry]->I[n,c,ox+rx,oy+ry]}
{S[n,k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[n,k,c,ox,oy,rx,ry]->O[n,k,ox,oy]}