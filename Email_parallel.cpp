#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <sstream>
#include <unordered_set>
#include <mpi.h>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Node {
    int id;
    list<pair<Node*, int>> connections;
    Node* previous;

    Node(int _id) : id(_id), previous(nullptr) {}
};

vector<Node*> readGraph(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        exit(1);
    }

    vector<Node*> graph;

    string line;
    while (getline(file, line)) {
        if (line[0] == '#' || line.empty()) continue;

        stringstream ss(line);
        int source, target;
        ss >> source >> target;

        if (source >= graph.size()) graph.resize(source + 1);
        if (target >= graph.size()) graph.resize(target + 1);

        if (!graph[source]) graph[source] = new Node(source);
        if (!graph[target]) graph[target] = new Node(target);

        int cost = 1;
        graph[source]->connections.push_back({graph[target], cost});
        graph[target]->connections.push_back({graph[source], cost});
    }

    file.close();

    return graph;
}

#include <omp.h> // Include OpenMP header

list<pair<vector<Node*>, int>> findKthShortestPaths(Node* source, Node* target, const list<Node*>& nodes, int k) {
    list<pair<vector<Node*>, int>> kShortestPaths;
    auto compare = [&](const auto& lhs, const auto& rhs) {
        return lhs.first > rhs.first;
    };
    priority_queue<pair<int, vector<Node*>>, vector<pair<int, vector<Node*>>>, decltype(compare)> pq(compare);

    pq.push({0, {source}});

    #pragma omp parallel
    {
        while (!pq.empty()) {
            vector<Node*> path;
            int pathCost;
            Node* lastNode;

            #pragma omp critical // Critical section to pop from priority queue
            {
                if (!pq.empty()) {
                    path = pq.top().second;
                    pathCost = pq.top().first;
                    pq.pop();
                    lastNode = path.back();
                }
            }

            if (!path.empty()) {
                if (lastNode == target) {
                    bool isDuplicate = false;
                    #pragma omp critical // Critical section to access kShortestPaths
                    {
                        for (const auto& p : kShortestPaths) {
                            if (p.first == path) {
                                isDuplicate = true;
                                break;
                            }
                        }

                        if (!isDuplicate) {
                            kShortestPaths.push_back({path, pathCost});
                            if (--k == 0)
                                break;
                        }
                    }
                }

                #pragma omp for // Parallelize the loop using an iterator
                for (auto it = lastNode->connections.begin(); it != lastNode->connections.end(); ++it) {
                    Node* nextNode = it->first;
                    int edgeCost = it->second;

                    if (find(path.begin(), path.end(), nextNode) == path.end()) {
                        vector<Node*> newPath = path;
                        newPath.push_back(nextNode);
                        int newCost = pathCost + edgeCost;
                        #pragma omp critical // Critical section to push to priority queue
                        {
                            pq.push({newCost, newPath});
                        }
                    }
                }
            }
        }
    }

    return kShortestPaths;
}


int main(int argc, char* argv[]) {
    const string filename = "1.txt";
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<Node*> graph = readGraph(filename);

    int sourceId = 1;
    int destId = 1504;
    int numPaths = 10;
    int k;

    if (rank == 0) {
        cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
        cout << "\t\t\t\t\t\t\t\t\t Kth Shortest Path\n\n";

        cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

        cout << "INPUT THE VALUE OF K : ";
        cin >> k;
        cout << "\n*--------------\n";

    }
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        srand(time(NULL)); // Seed the random number generator only once

        cout << graph.size() << endl;

        for (int p = 1; p < size; p++) {
            int sId = rand() % graph.size();
            int targetId = rand() % graph.size();

            cout << "SOURCE ->" << sId << " Target ->" << targetId << endl;

            MPI_Send(&sId, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
            MPI_Send(&targetId, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&sourceId, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&destId, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (sourceId >= graph.size() || destId >= graph.size()) {
        cerr << "Invalid source or target ID." << endl;
        MPI_Finalize();
        return 1;
    }

    Node* source = graph[sourceId];
    Node* dest = graph[destId];

    MPI_Win mutex;
    MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &mutex);
    int lock = 1;
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, mutex);

    cout << "\n==================================================================\n";

    double start_time = MPI_Wtime();

    list<Node*> nodes(graph.begin(), graph.end());

    list<pair<vector<Node*>, int>> kthShortestPaths = findKthShortestPaths(source, dest, nodes, k);

    cout << "Kth shortest paths from node " << sourceId << " to node " << destId << ":" << endl;
    for (const auto& path : kthShortestPaths) {
        cout << "Path (Cost: " << path.second << "): ";
        for (size_t j = 0; j < path.first.size(); ++j) {
            cout << path.first[j]->id;
            if (j != path.first.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
    }

    // Free memory
    for (Node* node : graph) {
        delete node;
    }

    double end_time = MPI_Wtime();
    double execution_time = end_time - start_time;

    cout << "Process " << rank << " execution time: " << fixed << setprecision(5) << execution_time << " seconds" << endl;

    MPI_Win_unlock(0, mutex);
    MPI_Win_free(&mutex);

    MPI_Finalize();

    return 0;
}
