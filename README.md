# parallel-distributed-computing-MPI-OPENMP

Your task is to implement a parallel solution for the Top K
Shortest Path Problem using MPI and OpenMP. Here's an outline of the steps to
follow:
o Input: Read the graph representation from a file. The graph can be a weighted
graph. You may need to perform pre-processing to adapt it according to the
requirements of the code given in the above link.
o Initialization: Initialize a distance matrix with the weights of the graph edges.
Initialize the diagonal elements to zero (distance of a vertex to itself) and
infinity for unreachable vertices.
o Parallelization Strategy: Divide the computation of the Kth shortest path
among MPI processes. Each MPI process will be responsible for exploring a
subset of paths. Within each MPI processes the loops shall be parallelized
using Openmp.
