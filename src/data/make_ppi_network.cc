#include <bits/stdc++.h>
using namespace std;
typedef vector<int> vi;

class UnionFind {
private:
  vi p, rank, setSize;
  int numSets;
public:
  UnionFind() {}
  UnionFind(int N) {
    setSize.assign(N, 1); numSets = N; rank.assign(N, 0);
    p.assign(N, 0); for (int i = 0; i < N; i++) p[i] = i; }
  int findSet(int i) { return (p[i] == i) ? i : (p[i] = findSet(p[i])); }
  bool isSameSet(int i, int j) { return findSet(i) == findSet(j); }
  void unionSet(int i, int j) { 
    if (!isSameSet(i, j)) { numSets--; 
    int x = findSet(i), y = findSet(j);
    if (rank[x] > rank[y]) { p[y] = x; setSize[x] += setSize[y]; }
    else                   { p[x] = y; setSize[y] += setSize[x];
                             if (rank[x] == rank[y]) rank[y]++; } } }
  int numDisjointSets() { return numSets; }
  int sizeOfSet(int i) { return setSize[findSet(i)]; }
};

map<int, map<int, double>> ReindexGraph(const map<int, map<int, double>>& graph) {
  map<int, int> reindexer;
  int num_nodes = 0;
  for (const auto& kv : graph) {
    int u = kv.first;
    reindexer[u] = num_nodes++;
  }
  assert(reindexer.size() == graph.size());
  map<int, map<int, double>> reindexed_graph;
  for (const auto& kv : graph) {
    int u = kv.first;
    for (const auto& kv2 : kv.second) {
      int v = kv2.first;
      double w = kv2.second;
      reindexed_graph[reindexer[u]][reindexer[v]] = w;
    }
  }
  assert(reindexed_graph.size() == graph.size());
  // Check that graph has node set {0, 1, ..., num_nodes - 1}.
  for (int i = 0; i < num_nodes; i++) {
    assert(reindexed_graph.find(i) != reindexed_graph.end());
  }
  return reindexed_graph;
}

// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
vector<string> StringSplit(const string& s, const string& delimiter) {
  vector<string> ret;
  int start = 0;
  int end = s.find(delimiter);
  string token;
  while (end != string::npos) {
    token = s.substr(start, end - start);
    start = end + delimiter.length();
    end = s.find(delimiter, start);
    ret.push_back(token);
  }
  token = s.substr(start, end);
  ret.push_back(token);
  return ret;
}

int StringToInt(const string& s) {
  stringstream ss(s);
  int ret;
  ss >> ret;
  return ret;
}

map<int, map<int, double>> ReadGraph(const string& input_filename) {
  ifstream file;
  file.open(input_filename);  // CSV with carriage returns.
  assert(file.is_open());
  map<int, map<int, double>> graph;
  string line;
  for (int i = 0; i < 5; i++) {  // Skip tokens on first line.
    file >> line;
  }
  while (file >> line) {
    const auto tokens = StringSplit(line, ",");
    assert(tokens.size() == 2);
    int u = StringToInt(tokens[0]);
    int v = StringToInt(tokens[1]);
    if (u == v) continue;  // Skip self loops.
    assert(u != v);
    graph[u][v] = 1.0;
    graph[v][u] = 1.0;
  }
  int num_nodes = graph.size();
  long long num_edges = 0;
  for (const auto& kv : graph) {
    for (const auto& node_weight : kv.second) {
      if (node_weight.first > kv.first) {
        num_edges++;
      }
    }
  }
  cout << "num_nodes: " << num_nodes << endl;
  cout << "num_edges: " << num_edges << endl;
  file.close();
  return ReindexGraph(graph);
}

bool IsConnected(const map<int, map<int, double>>& graph) {
  int num_nodes = graph.size();
  UnionFind uf(num_nodes);
  for (const auto& kv : graph) {
    int u = kv.first;
    for (const auto& kv2 : kv.second) {
      int v = kv2.first;
      uf.unionSet(u, v);
    }
  }
  return uf.numDisjointSets() == 1;
}

map<int, map<int, double>> GetLargestComponent(const map<int, map<int, double>>& graph) {
  int num_nodes = graph.size();
  UnionFind uf(num_nodes);
  for (const auto& kv : graph) {
    int u = kv.first;
    for (const auto& kv2 : kv.second) {
      int v = kv2.first;
      uf.unionSet(u, v);
    }
  }
  int largest_component_size = 0;
  int largest_component_parent = -1;
  cout << "num_components: " << uf.numDisjointSets() << endl;
  for (int i = 0; i < graph.size(); i++) {
    if (uf.findSet(i) == i) {
      //cout << " - component: " << i << " " << uf.sizeOfSet(i) << endl;
      if (uf.sizeOfSet(i) > largest_component_size) {
        largest_component_size = uf.sizeOfSet(i);
        largest_component_parent = i;
      }
    }
  }
  assert(largest_component_parent != -1);
  set<int> nodes_in_largest;
  for (int i = 0; i < graph.size(); i++) {
    if (uf.findSet(i) == largest_component_parent) {
      nodes_in_largest.insert(i);
    }
  }
  assert(nodes_in_largest.size() == largest_component_size);
  map<int, map<int, double>> largest_component;
  for (const auto& kv : graph) {
    int u = kv.first;
    if (!nodes_in_largest.count(u)) continue;
    for (const auto& kv2 : kv.second) {
      int v = kv2.first;
      double w = kv2.second;
      if (nodes_in_largest.count(v)) {
        largest_component[u][v] = w;
      }
    }
  }
  return ReindexGraph(largest_component);
}

void WriteGraph(const map<int, map<int, double>>& graph,
                const string& output_filename) {
  ofstream file;
  file.open(output_filename);
  assert(file.is_open());
  int num_nodes = graph.size();
  long long num_edges = 0;
  for (const auto& kv : graph) {
    for (const auto& node_weight : kv.second) {
      if (node_weight.first > kv.first) {
        num_edges++;
      }
    }
  }
  file << num_nodes << " " << num_nodes << " " << num_edges << endl;
  for (const auto& kv : graph) {
    for (const auto& node_weight : kv.second) {
      if (node_weight.first > kv.first) {
        file << kv.first << " " << node_weight.first << " "
             << node_weight.second << endl;
      }
    }
  }
  file.close();
}

int main() {
  // http://snap.stanford.edu/pathways/ 
  const string input_filename = "bio-pathways-network.csv";
  const string output_filename = "ppi.network.txt";
  auto graph = ReadGraph(input_filename);
  graph = GetLargestComponent(graph);
  cout << "connected: " << IsConnected(graph) << endl;
  WriteGraph(graph, output_filename);
  return 0;
}
