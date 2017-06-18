# assignment 3
This assignment is basically identical to assignment 2 except we are testing on the RPI supercomputer at the Center for Computational Innovations (CCI).
The supercomputer in question is a Blue Gene/Q named ```amos```. The test scripts are named experiment1-x.sh and experiment2.sh, respectively.
I thought the results would be interesting, so here they are!

## experiment 1 test results
All tests are for an 8192x8192 board with 100 ticks.

### 0% threshold
MPI Ranks | BG/Q Nodes | Execution Time (s)
--- | --- | ---
64 | 1 | 41.39
128 | 2 | 20.81
256 | 4 | 10.42
512 | 8 | 5.227
1024 | 16 | 2.523
2048 | 32 | 1.959
4096 | 64 | 1.021
8192 | 128 | 0.836

### 25% threshold
MPI Ranks | BG/Q Nodes | Execution Time (s)
--- | --- | ---
64 | 1 | 52.17
128 | 2 | 24.14
256 | 4 | 12.68
512 | 8 | 5.443
1024 | 16 | 2.194
2048 | 32 | 1.849
4096 | 64 | 1.177
8192 | 128 | 0.748

### 50% threshold
MPI Ranks | BG/Q Nodes | Execution Time (s)
--- | --- | ---
64 | 1 | 40.49
128 | 2 | 22.91
256 | 4 | 10.91
512 | 8 | 6.122
1024 | 16 | 1.898
2048 | 32 | 1.529
4096 | 64 | 0.893
8192 | 128 | 0.572

### 75% threshold
MPI Ranks | BG/Q Nodes | Execution Time (s)
--- | --- | ---
64 | 1 | 36.53
128 | 2 | 20.17
256 | 4 | 11.02
512 | 8 | 4.303
1024 | 16 | 1.994
2048 | 32 | 1.413
4096 | 64 | 0.881
8192 | 128 | 0.477

### experiment 2 test results
All tests are for an 8192x8192 board with 100 ticks.
This experiment uses 32 MPI ranks / node instead of 64.

### 50% threshold
MPI Ranks | BG/Q Nodes | Execution Time (s)
--- | --- | ---
32 | 1 | 66.81
64 | 2 | 33.49
128 | 4 | 19.67
256 | 8 | 7.467
512 | 16 | 5.041
1024 | 32 | 2.618
2048 | 64 | 1.363
4096 | 128 | 0.725