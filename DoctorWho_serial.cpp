#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

int numNodes = 0;
struct Node {
    string name;
    list<pair<Node*, int>> connections;
    int cost;
    Node* prev;

    Node(const string& n) : name(n), cost(-1), prev(nullptr) {}
};

// Function to find a node by its name
Node* findNodeByName(const list<Node*>& nodes, const string& name) {
    for (Node* node : nodes) {
        if (node->name == name) {
            return node;
        }
    }
    return nullptr;
}

// Function to read CSV file and create nodes and connections
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

// Function to find k-th shortest paths
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
            if (--k == 0)
                break;
        }

        for (auto it = lastNode->connections.begin(); it != lastNode->connections.end(); ++it) {
            Node* nextNode = it->first;
            int edgeCost = it->second;

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

int main(int argc, char* argv[]) {
    string filename = "doctorwho.csv";

    list<Node*> nodes;
    readCSVFile(filename, nodes);

    int k;

    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "\t\t\t\t\t\t\t\t\t Kth Shortest Path\n\n";
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

    cout << "INPUT THE VALUE OF K : ";
    cin >> k;
    cout << "\n---------------------------*\n";

     string sourceName[9] = {"Tom Osgood", "Dalek Supreme (Planet of the Daleks)", "Xanxia", "Bonnie (The Zygon Invasion)", "BOSS", "Richard Lazarus", "Vicki Pallister", "Lupton", "Will Chandler"};
    string targetName[9] = {"Tibo", "Ike Clanton", "Grand Marshall", "Agatha Christie", "Seventh Doctor", "Harry Sullivan", "Dorium Maldovar", "White Guardian", "Reegan"};
    
    
    srand(time(NULL)); // Seed the random number generator only once

    for (int p = 0; p < 9; p++) {
        Node* sourceNode = findNodeByName(nodes, sourceName[p]);
        Node* targetNode = findNodeByName(nodes, targetName[p]);

        if (!sourceNode) {
            cout << "Source node " << sourceName[p] << " not found!" << endl;
            continue; // Skip to the next iteration
        }

        if (!targetNode) {
            cout << "Target node " << targetName[p] << " not found!" << endl;
            continue; // Skip to the next iteration
        }

        for (Node* node : nodes) {
            node->cost = -1;
            node->prev = nullptr;
        }
        sourceNode->cost = 0;

        clock_t start_time = clock(); // Start the timer
        
        list<pair<vector<Node*>, int>> kthShortestPaths = findKthShortestPaths(sourceNode, targetNode, nodes, k);

        clock_t end_time = clock(); // End the timer
        double execution_time = double(end_time - start_time) / CLOCKS_PER_SEC; // Calculate execution time

        cout << "\n==================================================================\n";
        cout << "Kth shortest paths from '" << sourceNode->name << "' to '" << targetNode->name << "':" << endl << endl;
        for (const auto& path : kthShortestPaths) {
            cout << "Path (Cost: " << path.second << "): ";
            for (size_t j = 0; j < path.first.size(); ++j) {
                cout << path.first[j]->name;
                if (j != path.first.size() - 1) {
                    cout << " -> ";
                }
            }
            cout << endl << endl;
        }
        cout << "Execution time: " << fixed << setprecision(5) << execution_time << " seconds" << endl;
    }

    for (Node* node : nodes) {
        delete node;
    }

    return 0;
}
