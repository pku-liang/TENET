2 1
{S[k,c,ox,oy,rx,ry]:0<=k<64 and 0<=c<3 and 0<=ox<112 and 0<=oy<112 and 0<=rx<7 and 0<=ry<7}
{S[k,c,ox,oy,rx,ry]->I[c,ox*2+rx,oy*2+ry]}
{S[k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[k,c,ox,oy,rx,ry]->O[k,ox,oy]}