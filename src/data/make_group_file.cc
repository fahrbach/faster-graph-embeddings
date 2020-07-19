#include <bits/stdc++.h>
using namespace std;

map<int, map<int, double>> ReadGraph(const string& input_filename) {
  ifstream file;
  file.open(input_filename);
  assert(file.is_open());
  map<int, map<int, double>> graph;
  int num_nodes, num_edges;
  // Read first line.
  file >> num_nodes >> num_nodes >> num_edges;
  int u, v;
  double w;
  while (file >> u >> v >> w) {  // Expects edge weight.
    graph[u][v] = w;
    graph[v][u] = w;
  }
  // Check that graph has node set {0, 1, ..., num_nodes - 1}.
  for (int i = 0; i < num_nodes; i++) {
    assert(graph.find(i) != graph.end());
  }
  file.close();
  return graph;
}

set<int> ReadSubset(const string& input_filename) {
  ifstream file;
  file.open(input_filename);
  assert(file.is_open());
  set<int> terminals;
  int num_terminals, num_nodes;
  file >> num_terminals >> num_nodes;
  for (int i = 0; i < num_terminals; i++) {
    int node;
    file >> node;
    assert(0 <= node && node < num_nodes);
    terminals.insert(node);
  }
  file.close();
  assert(terminals.size() == num_terminals);
  return terminals;
}

void WriteGroup(const string& output_filename,
                const map<int, map<int, double>>& graph,
                const set<int>& terminals) {
  ofstream file;
  file.open(output_filename);
  assert(file.is_open());
  int num_nodes = graph.size();
  int num_terminals = terminals.size();
  file << num_nodes << " " << 1 << " " << num_terminals << endl;
  for (const auto& node : terminals) {
    file << node << " " << 0 << endl;
  }
  file.close();
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    cout << "Usage: make_group_file input_network_filename "
            "input_subset_filename output_group_filename" << endl;
    return 1;
  }
  const string input_network_filename = string(argv[1]);
  const string input_subset_filename = string(argv[2]);
  const string output_group_filename = string(argv[3]);

  const auto graph = ReadGraph(input_network_filename);
  const auto terminals = ReadSubset(input_subset_filename);
  WriteGroup(output_group_filename, graph, terminals);
  return 0;
}
