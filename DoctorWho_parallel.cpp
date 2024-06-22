#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <omp.h>
#include<iomanip>


using namespace std;

int numNodes=0;
struct Node {
    string name;
    list<pair<Node*, int>> connections; 
    int cost;
    Node* prev;

    Node(const string& n) : name(n), cost(-1), prev(nullptr) {}
};

Node* findNodeByName(const list<Node*>& nodes, const string& name) {
    for (Node* node : nodes) {
        if (node->name == name) {
            return node;
        }
    }
    return nullptr;
}

void readCSVFile(const string& filename, list<Node*>& nodes) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        exit(1);
    }

    string line;
    bool isFirstLine = true;

    while (getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }

        stringstream ss(line);
        string source, target;
        int cost;
        getline(ss, source, ',');
        getline(ss, target, ',');
        ss >> cost;

        Node* sourceNode = findNodeByName(nodes, source);
        if (!sourceNode) {
            sourceNode = new Node(source);
            nodes.push_back(sourceNode);
            numNodes++;
        }

        Node* targetNode = findNodeByName(nodes, target);
        if (!targetNode) {
            targetNode = new Node(target);
            nodes.push_back(targetNode);
            numNodes++;
        }

        sourceNode->connections.push_back({targetNode, cost});
    }

    file.close();
}


list<pair<vector<Node*>, int>> findKthShortestPaths(Node* source, Node* target, const list<Node*>& nodes, int k) {
    list<pair<vector<Node*>, int>> kShortestPaths;
    auto compare = [&](const auto& lhs, const auto& rhs) {
        return lhs.first > rhs.first;
    };
    priority_queue<pair<int, vector<Node*>>, vector<pair<int, vector<Node*>>>, decltype(compare)> pq(compare);

    pq.push({0, {source}});

    while (!pq.empty()) {
        vector<Node*> path = pq.top().second;
        int pathCost = pq.top().first;
        pq.pop();

        Node* lastNode = path.back();

        if (lastNode == target) {
            kShortestPaths.push_back({path, pathCost});
            #pragma omp critical
            {
                if (--k == 0)
                    break;
            }
        }

        #pragma omp parallel
        {
            #pragma omp for
            for (auto it = lastNode->connections.begin(); it != lastNode->connections.end(); ++it) {
                Node* nextNode = it->first;
                int edgeCost = it->second;

                if (find(path.begin(), path.end(), nextNode) == path.end()) {
                    vector<Node*> newPath = path;
                    newPath.push_back(nextNode);

                    int newCost = pathCost + edgeCost;

                    #pragma omp critical
                    pq.push({newCost, newPath});
                }
            }
        }
    }

    return kShortestPaths;
}



int main(int argc, char *argv[]) {
    string filename = "doctorwho.csv"; 
    
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    list<Node*> nodes;
    readCSVFile(filename, nodes);

    int numPaths = 10;
      int k; 
    
  if (rank ==0)
  {  
    cout<<"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    cout<<"\t\t\t\t\t\t\t\t\t Kth Shortest Path\n\n";
    
    cout<<"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    
    cout<<"INPUT THE VALUE OF K : ";
    cin>>k;
    cout<<"\n---------------------------*\n";
    
  }
  MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
  Node sourceIndex("Ace"), targetIndex("Adric");

 string sourceName, targetName;
    if (rank == 0) {
        

        srand(time(NULL)); 

	for (int p = 1; p < size; p++) {
	    int sourceIndex = rand() % numNodes;
	    int targetIndex = rand() % numNodes;

	    auto it = nodes.begin();
	    advance(it, sourceIndex);
	    sourceName = (*it)->name;

	    it = nodes.begin();
	    advance(it, targetIndex);
	    targetName = (*it)->name;

	    cout << "SOURCE ->" << sourceName << " Target ->" << targetName << endl;

	    MPI_Send(sourceName.c_str(), sourceName.size() + 1, MPI_CHAR, p, 0, MPI_COMM_WORLD);
	    MPI_Send(targetName.c_str(), targetName.size() + 1, MPI_CHAR, p, 0, MPI_COMM_WORLD);
	}

    	} else {
       		char sourceNameBuffer[256], targetNameBuffer[256];
        	MPI_Recv(sourceNameBuffer, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        	MPI_Recv(targetNameBuffer, 256, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        	sourceIndex.name = string(sourceNameBuffer);
        	targetIndex.name = string(targetNameBuffer);
    	}



    
    Node* sourceNode = findNodeByName(nodes, sourceIndex.name);
    Node* targetNode = findNodeByName(nodes, targetIndex.name);

    if (!sourceNode || !targetNode) {
        cout << "Source or target node not found!" << endl;
        return 1;
    }

    for (Node* node : nodes) {
        node->cost = -1;
        node->prev = nullptr;
    }
    sourceNode->cost = 0;

    list<pair<vector<Node*>, int>> kthShortestPaths = findKthShortestPaths(sourceNode, targetNode, nodes, k);
    
    MPI_Win mutex;
    MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &mutex);
    int lock = 1;
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, mutex);
    
        cout<<"\n==================================================================\n";
double start_time = MPI_Wtime(); 

    cout << "Kth shortest paths from '" << sourceNode->name << "' to '" << targetNode->name << "':" << endl<<endl;
	
	for (const auto& path : kthShortestPaths) {
        cout << "Path (Cost: " << path.second << "): ";
        for (size_t j = 0; j < path.first.size(); ++j) {
            cout << path.first[j]->name;
            if (j != path.first.size() - 1) {
                cout << " -> ";
            }
        }
        cout << endl;
        cout<<endl;
    }
    double end_time = MPI_Wtime();
    double execution_time = end_time - start_time;
 
cout << "Process " << rank << " execution time: " << fixed << setprecision(5) << execution_time << " seconds" << endl;
    MPI_Win_unlock(0, mutex);
    MPI_Win_free(&mutex);
    
    

    for (Node* node : nodes) {
        delete node;
    }

    MPI_Finalize();
    

    return 0;
}
