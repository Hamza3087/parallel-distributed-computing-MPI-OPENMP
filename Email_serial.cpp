#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <sstream>
#include <unordered_set>
#include <algorithm>
#include<iomanip>

#include <ctime>
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
            bool isDuplicate = false;
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

        for (const auto& neighbor : lastNode->connections) {
            Node* nextNode = neighbor.first;
            int edgeCost = neighbor.second;

            if (find(path.begin(), path.end(), nextNode) == path.end()) {
                vector<Node*> newPath = path;
                newPath.push_back(nextNode);
                int newCost = pathCost + edgeCost;
                pq.push({newCost, newPath});
            }
        }
    }

    return kShortestPaths;
}


int main() {

    const string filename = "1.txt";
    
    vector<Node*> graph = readGraph(filename);

    int numPaths = 10;
    int k;
    
    
cout<<"\n==================================================================\n";
    
    int sourceId[10] = {1,17,1128,1803,396,1430,2831,603,1199,218},
    distId[10] = {1504,329,1786,3278,975,2251,1348,650,2888,2767};

      cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
      cout << "\t\t\t\t\t\t\t\t\t Kth Shortest Path\n\n";

      cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

      cout << "INPUT THE VALUE OF K : ";
      cin >> k;
      cout << "\n*--------------\n";

   

        for (int p = 0; p < 10; p++) {
            int sId = sourceId[2];
            int targetId = distId[2];

            

            cout << "SOURCE ->" << sId << " Target ->" << targetId << endl;

           
    

    Node* source = graph[sId];
    Node* dest = graph[targetId];
    
   
  
    
            clock_t start_time = clock(); // Start the timer


    list<Node*> nodes(graph.begin(), graph.end());

    list<pair<vector<Node*>, int>> kthShortestPaths = findKthShortestPaths(source, dest, nodes, k);
    
      clock_t end_time = clock(); // End the timer
        double execution_time = double(end_time - start_time) / CLOCKS_PER_SEC; // Calculate execution time


    cout << "Kth shortest paths from node " << sId << " to node " << targetId << ":" << endl;
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

 cout << "Execution time: " << fixed << setprecision(5) << execution_time << " seconds" << endl;
 
 
	cout<<"\n==================================================================\n";

}
    

    // Free memory
    for (Node* node : graph) {
        delete node;
    }
    
    return 0;
}
