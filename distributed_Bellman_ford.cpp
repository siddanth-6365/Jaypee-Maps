#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <climits>
#include <sstream>

using namespace std;

struct Edge
{
    int u, v, w;
};

int V = 0, E = 0;
vector<Edge> edges;
vector<string> locationNames;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    if (rank == 0)
    {
        ifstream locFile("locations.txt");
        if (!locFile.is_open())
        {
            cerr << "Error opening locations.txt\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        string line;
        while (getline(locFile, line))
        {
            stringstream ss(line);
            int nodeId;
            string name;
            ss >> nodeId;
            getline(ss, name);
            while (!name.empty() && name[0] == ' ') 
                name.erase(name.begin());
            if (nodeId > V) 
                V = nodeId;
            if ((int)locationNames.size() < nodeId) 
                locationNames.resize(nodeId);
            locationNames[nodeId - 1] = name;
        }
        locFile.close();

        ifstream distFile("dist_matrix.txt");
        if (!distFile.is_open())
        {
            cerr << "Error opening dist_matrix.txt\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        int u, v, w;
        while (distFile >> u >> v >> w)
        {
            edges.push_back({u - 1, v - 1, w});
            edges.push_back({v - 1, u - 1, w});
            E++;
        }
        distFile.close();
    }

    MPI_Bcast(&V, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&E, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) 
        locationNames.resize(V);
    for (int i = 0; i < V; i++)
    {
        int len = 0;
        if (rank == 0) 
            len = locationNames[i].size();
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank != 0) 
            locationNames[i].resize(len);
        MPI_Bcast(&locationNames[i][0], len, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    if (rank != 0) 
        edges.resize(E);
    MPI_Bcast(edges.data(), E * sizeof(Edge), MPI_BYTE, 0, MPI_COMM_WORLD);

    int nodesPerProc = V / numProcs;
    int startNode = rank * nodesPerProc;
    int endNode = (rank == numProcs - 1) ? V : (rank + 1) * nodesPerProc;

    vector<int> localDist(V, INT_MAX);
    vector<int> parent(V, -1);

    int sourceNode = 1, destinationNode = 6;
    MPI_Bcast(&sourceNode, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&destinationNode, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (startNode <= sourceNode && sourceNode < endNode)
        localDist[sourceNode] = 0;

    for (int i = 0; i < V - 1; i++)
    {
        vector<int> globalDist(V);
        MPI_Allgather(&localDist[startNode], nodesPerProc, MPI_INT,
                      &globalDist[0], nodesPerProc, MPI_INT, MPI_COMM_WORLD);

        bool changed = false;
        for (const auto &edge : edges)
        {
            if (startNode <= edge.u && edge.u < endNode)
            {
                if (globalDist[edge.u] != INT_MAX &&
                    globalDist[edge.u] + edge.w < localDist[edge.v])
                {
                    localDist[edge.v] = globalDist[edge.u] + edge.w;
                    parent[edge.v] = edge.u;
                    changed = true;
                }
            }
        }

        bool globalChange;
        MPI_Allreduce(&changed, &globalChange, 1, MPI_C_BOOL, MPI_LOR, MPI_COMM_WORLD);
        if (!globalChange) 
            break;
    }

    vector<int> finalDist(V);
    vector<int> finalParent(V);

    MPI_Gather(&localDist[startNode], nodesPerProc, MPI_INT,
               &finalDist[0], nodesPerProc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&parent[startNode], nodesPerProc, MPI_INT,
               &finalParent[0], nodesPerProc, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "Shortest distance from (" << sourceNode + 1 << ") "
             << locationNames[sourceNode] << " to (" << destinationNode + 1 << ") "
             << locationNames[destinationNode] << " is: ";

        if (finalDist[destinationNode] == INT_MAX)
        {
            cout << "INF (No Path)\n";
        }
        else
        {
            cout << finalDist[destinationNode] << "\n";

            vector<int> path;
            int current = destinationNode;
            while (current != -1)
            {
                path.push_back(current);
                if (current == sourceNode) 
                    break;
                current = finalParent[current];
            }
            reverse(path.begin(), path.end());

            cout << "Path: ";
            for (size_t i = 0; i < path.size(); i++)
            {
                cout << "(" << path[i] + 1 << ") " << locationNames[path[i]];
                if (i < path.size() - 1) 
                    cout << " -> ";
            }
            cout << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
