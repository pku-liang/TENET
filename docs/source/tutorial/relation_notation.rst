Relation Notation Get Started
=============================


1. Describe a statement
-----------------------
Multiplication of two matrixes A and B size 512*512, can be written in C:

.. code-block:: C

    int A[512][512], B[512][512], C[512][512];
    for (int i = 0; i < 512; i++) {          // first level loop
        for (int j = 0; j < 512; j++) {      // second level loop
            for (int k = 0; k < 512; k++) {  // third level loop
                C[i][j] += A[i][k] * B[k][j]; // inner-most statement
            }
        }
    }


We can refer every statement as ``S[i][j][k]``. (i, j, k are corresponding loop variables)
We call every ``S[.][.][.]`` a **statement instance**.
``S[i][j][k]`` uses ``A[i][k]`` and ``B[k][j]`` to update ``C[i][j]``, so we can get mappings of *instance -> data access*:

.. code-block:: none

    {S[i][j][k]->A[i][k]}, {S[i][j][k]->B[k][j]}, {S[i][j][k]->C[i][j]}

2. Execution on a single-core
-----------------------------

Suppose we have a single-core processor, and our compiler doesn't reorder the loops. Then the program will execute i\*j\*k multiply-and-add statements sequentially.
In this way, statement instance maps to execution sequence straightforward::

    {S[i][j][k]->T[i][j][k]}

Later we know with the existence of memory hierarchy, the higher level cache you load data from, the more computation efficiency you get.
So we can arrange the statement instance order wiser, like loop reorder and data blocking, to exploit time-locality and space-locality.
The refined code should look like this:

.. code-block:: C

    int A[512][512], B[512][512], C[512][512];
    // loop specifying block level computation order 
    for (int i1 = 0; i1 < 64; i1++) {
        for (int j1 = 0; j1 < 64; j1++) {
            for (int k1 = 0; k1 < 64; k1++) {
    // loop specifying intra-block computation order
                for (int i2 = 0; i2 < 8; i2++) {
                    for (int j2 = 0; j2 < 8; j2++) {
                        for (int k2 = 0; k2 < 8; k2++) {
                            C[i1 * 8 + i2][j1 * 8 + j2] += A[i1 * 8 + i2][k1 * 8 + k2] * B[k1 * 8 + k2][j1 * 8 + j2];
                        }
                    }
                }
            }
        }
    }

As every original loop variable X divided into two new variables, X1 and X2, *instance->time* mapping changes to::

    {S[i][j][k]->T[i1][j1][k1][i2][j2][k2]}    
    where
    i = i1 * 8 + i2; j = i1 * 8 + j2; k = k1 * 8 + k2;

or simplified::

    {S[i][j][k]->T[floor(i/8)][floor(j/8)][floor(k/8)][i%8][j%8][k%8]}

3. Map onto parallel architecture
---------------------------------

Now suppose we have a chip specialized for GEMM acceleration with 8\*8 grid PE on it. We can compute one block in parrallel.
First describe this hardware structure as::
    
    {PE[i][j]: 0<=i<8; 0<=j<8} 

Then map 8\*8 intra-block computation onto the grid::

    {S[i][j][k]->PE[i%8][j%8]}

The least-imporant three time stamps are eliminated::

    {S[i][j][k]->T[floor(i/8)][floor(j/8)][floor(k%8)][k%8]}

This dataflow style decomposes intra-block computation into a sequence of out-product operation.
For every PE, data is fed directly from on-chip buffer, so the wire length from on-chip buffer to the furthest PE is very long, limiting the clock frequency.
A better dataflow is **systolic array**, which pumps data into the PE array one by one, and reuse data as it flows towards bottom or right side. In this way, *instance->time* mapping has affine-transformation of i, j, k in the least important coordinate::

    S[i][j][k]->PE[i%8][j%8]
    S[i][j][k]->T[floor(i/8)][floor(j/8)][floor(k/8)][i%8+j%8+k%8]

We also need to specify PE interconnection, as it enables direct data transfer between adjcent PEs::

    {PE[i][j]->PE[i+1][j]; PE[i][j]->PE[i][j+1]}

4. Summary
----------
Now we have already touched three most important abstractions of TENET: *Statement*, *Hardware Architecture(PE Interconnect)*, *Mapping*.
They are the language used by TENET core-functions to describe and analyze dataflow of tensor computation.

You have choice to directly go to :doc:`usage`, where you have opportunity to write your own TENET input files (They are actually knowledge you learned in this page with a more standard format) and run it to get the result.
Or you can follow the next page to see how relation notation is used in metrics computing like reuse factor, PE-utilization, and Latency.
