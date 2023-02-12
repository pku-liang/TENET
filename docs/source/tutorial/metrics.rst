Metrics Computation
===================

We continue to use GEMM for two 512*512 matrix as example::

    Statement:
    {S[i][j][k]: 0<=i<512 and 0<=j<512 and 0<=k<512}
    {S[i][j][k]->A[i][k]}
    {S[i][j][k]->B[k][j]}
    {S[i][j][k]->C[i][j]}
    
    PE Interconnect:
    {PE[i][j]: 0<=i<8 and 0<=j<8}
    {PE[i][j]->PE[i+1][j]; PE[i][j]->PE[i][j+1]}

    Mapping:
    {S[i][j][k]->PE[i%8][j%8]}
    {S[i][j][k]->T[floor(i/8)][floor(j/8)][floor(k/8)][i%8+j%8+k%8]}


Assume we want to compute some metrics of input matrix A
First, we need to recongnize which groups of statement instance use the same data for computation.
TENET can find any two statement instances S[i][j][k] and S[i][j'][k] access the same A[i][k], and construct a *statement->statement* mapping {S[i][j][k]->S[i][j'][k]}

Then TENET constructs a *time->time* mapping to express any two timestamps are adjcent, and we denote it *{T[time_t]->T[adjacent_t]}* (for example T[0][1]->T[0][2], T[0][59]->T[1][0] are two nearby-time pair if T[i][j] denotes minute j, hour i)
As well, we construct two Identical mapping: {PE[i][j]->PE[i][j]}, {T[same_time]->T[same_time]}

time-space 


| As we add some constraints: 
| *space1->space2* Map equals ``Identical(Coordinate(PE)->Coordinate(PE)) union PE interconnection``;
| *time1->time2* Map equals ``Identical(Coordinate(T)->Coordinate(T)) union Coordinate(t1)->Coordinate(t2) s.t. t2 is immediately after t1 in canonical order``.
| Then construct *space1-time1->space2-time2*. Because

