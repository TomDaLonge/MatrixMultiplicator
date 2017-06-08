# MatrixMultiplicator
Parallel implementation of matrix multiplication using Microsoft MPI

# Course
Intelligent Systems Design / 6. Semester

# Description
The multiplication of two matrices is a task that can be effectively parallelized. As processors will not become much faster in frequency but instead will grow into width with more cores and processes running at the same time, this becomes more and more important. Practically this is very interesting especially for the computations inside neural networks.
This implementation creates two matrices filled with pseudorandom integers and multiplies them in *n* different processes on your CPU.

# Execute
mpiexex -n
n = Count of wanted processes

1. Parameter: Seed
2. Parameter: LogLevel
3. Parameter: CommonSize
4. Parameter: ResultWidth
5. Parameter: ResultHeight

# API used
Microsoft MPI
