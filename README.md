# Distributed Bellman-Ford Algorithm using MPI

## Overview
This project implements a **Distributed Bellman-Ford Algorithm** using **MPI (Message Passing Interface)** for finding the shortest path between two locations in a graph. The graph represents a **campus navigation system**, where locations are nodes and distances between them are edges.

## Features
- **Parallel Processing**: Distributes graph processing across multiple MPI processes.
- **Shortest Path Calculation**: Implements Bellman-Ford algorithm in a distributed manner.
- **Dynamic Graph Input**: Reads location names from `locations.txt` and distances from `dist_matrix.txt`.
- **Scalability**: Can efficiently handle large graphs with multiple processes.

## Dependencies
- OpenMPI (for running MPI-based programs)
- C++ Compiler (e.g., g++)

## Installation
### 1. Install OpenMPI
#### **MacOS (Homebrew)**
```bash
brew install open-mpi
```
#### **Ubuntu/Debian**
```bash
sudo apt update
sudo apt install openmpi-bin openmpi-common libopenmpi-dev
```
#### **Windows (WSL Recommended)**
Use Ubuntu in WSL and install OpenMPI as above.

## Compilation
```bash
mpic++ -std=c++11 distributed_bellman_ford.cpp -o dgp
```

## Running the Program
Run the program with **multiple processes (e.g., 4 processes)**:
```bash
mpirun -np 4 ./dgp
```

## Input Files
### **1. locations.txt** (Node ID to Location Name mapping)
```
1 ABB-III
2 ABB-I
3 JBS
4 GROUND
5 OAT
6 GATE-1
7 GATE-2
```

### **2. dist_matrix.txt** (Graph edges with distances)
```
1 2 100
1 3 100
1 4 50
1 5 60
1 6 80
1 7 20
```

## How It Works
1. **Process 0 reads the input files** and broadcasts the graph data to all MPI processes.
2. **Graph nodes are distributed** across processes for parallel computation.
3. **Bellman-Ford Algorithm runs iteratively**, updating distances in a distributed manner.
4. **Processes exchange shortest path updates** using `MPI_Allgather` and `MPI_Allreduce`.
5. **The shortest path is gathered at Process 0** and displayed.

## Output Example
```
Shortest distance from (1) ABB-III to (7) GATE-2 is: 20
Path: (1) ABB-III -> (7) GATE-2
```

## Troubleshooting
### **1. MPI Not Found?**
Check if OpenMPI is installed:
```bash
which mpic++
```
If not found, reinstall OpenMPI.

### **2. "Hangs After Execution"?**
- Ensure `locations.txt` and `dist_matrix.txt` exist.
- Run with **1 process first** to debug:
```bash
mpirun -np 1 ./dgp
```

## Future Improvements
- Implement **Dijkstra’s Algorithm** for faster execution.
- Add **real-time path updates** using dynamic graphs.
- Optimize **memory usage** for handling larger datasets.

## License
This project is open-source and free to use.
