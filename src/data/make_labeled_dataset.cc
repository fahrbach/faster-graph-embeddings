#include <bits/stdc++.h>
using namespace std;
typedef vector<int> vi;
typedef pair<int, int> ii;

const double THETA = 0.90;
const int TARGET_DEGREE = 30;

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

int n, m, num_label_types;  // m counts the number of undirected edges
vector<map<int, double>> adj;  // adjacency list
map<int, double> weighted_degree, slack_degree;
map<int, int> node_indexer, label_indexer;  // maps to 0,1,2,...,n-1
UnionFind cluster;
map<int, set<int>> node_to_label;
set<int> eliminated_set;

int reduced_n, reduced_m;  // variables for the reduced graph
vector<map<int, double>> reduced_adj;
map<int, int> reduced_node_indexer, reduced_label_indexer;

// -----------------------------------------------------------------------------
// Input
// -----------------------------------------------------------------------------

vector<string> Tokenize(string buffer) {
  vector<string> ret;
  stringstream ss(buffer);
  string token;
  while (ss >> token) ret.push_back(token);
  return ret;
}

int StringToInt(string s) {
  stringstream ss(s);
  int ret;
  ss >> ret;
  return ret;
}

double StringToDouble(string s) {
  stringstream ss(s);
  double ret;
  ss >> ret;
  return ret;
}

void ReadGraph(string filename, bool debug=false) {
  adj.clear();
  node_indexer.clear();
  int num_nodes = 0;
  int num_edges = 0;
  string buffer;
  ifstream fin(filename);
  assert(fin.is_open());
  getline(fin, buffer); // skip first line
  map<ii, double> edges;
  while (getline(fin, buffer)) {
    auto tokens = Tokenize(buffer);
    assert(tokens.size() == 2 || tokens.size() == 3);
    int u = StringToInt(tokens[0]);
    int v = StringToInt(tokens[1]);
    if (u == v) continue;
    double w = 1.0;
    if (tokens.size() == 3) w = StringToDouble(tokens[2]);
    edges[ii(u, v)] = w;
    num_edges++;
    if (!node_indexer.count(u)) node_indexer[u] = num_nodes++;
    if (!node_indexer.count(v)) node_indexer[v] = num_nodes++;
  }
  n = num_nodes;
  m = num_edges;
  cluster = UnionFind(n);
  if (debug) cout << "num_nodes: " << n << "\tnum_edges: " << m << endl;
  adj = vector<map<int, double>>(n);
  for (auto kv : edges) {
    int u = node_indexer[kv.first.first];
    int v = node_indexer[kv.first.second];
    double w = kv.second;
    adj[u][v] = w;
    adj[v][u] = w;
    cluster.unionSet(u, v);
  }
  assert(cluster.numDisjointSets() == 1);
  assert(node_indexer.size() == num_nodes);
}

void ReadLabels(string filename, bool debug=false) {
  node_to_label.clear();
  num_label_types = 0;
  string buffer;
  ifstream fin(filename);
  assert(fin.is_open());
  getline(fin, buffer);  // skip first line
  while (getline(fin, buffer)) {
    auto tokens = Tokenize(buffer);
    assert(tokens.size() == 2);
    int u = StringToInt(tokens[0]);
    assert(node_indexer.count(u));
    int label = StringToInt(tokens[1]);
    if (!label_indexer.count(label)) label_indexer[label] = num_label_types++;
    node_to_label[node_indexer[u]].insert(label_indexer[label]);
  }
}

set<ii> spanning_tree;
void FindSpanningTree() {
  UnionFind uf(n);
  vector<ii> edges;
  for (int i = 0; i < adj.size(); i++) {
    for (const auto& kv : adj[i]) {
      int j = kv.first;
      if (i < j) {
        edges.push_back(ii(i, j));
      }
    }
  }
  random_shuffle(edges.begin(), edges.end());
  for (const auto& edge : edges) {
    int u = edge.first;
    int v = edge.second;
    if (!uf.isSameSet(u, v)) {
      spanning_tree.insert(edge);
      uf.unionSet(u, v);
    }
  }
  assert(uf.numDisjointSets() == 1);
}

map<ii, int> sampled_edges;
void SampleTerminalAndNonterminalEdges(double ratio, bool debug=false) {
  // Find terminal nodes
  set<int> terminal_nodes;
  for (int i = 0; i < adj.size(); i++) {
    if (node_to_label[i].size()) {
      terminal_nodes.insert(i);
    }
  }
  if (debug) cout << "num_terminal_nodes: " << terminal_nodes.size() << endl;
  
  // Partition edges and non-edges incident to terminal nodes
  vector<ii> terminal_edges;
  vector<ii> terminal_non_edges;
  for (auto u : terminal_nodes) {
    for (auto v : terminal_nodes) {
      if (u >= v) continue;
      if (adj[u].count(v)) terminal_edges.push_back({u, v});
      else terminal_non_edges.push_back({u, v});
    }
  }
  if (debug) {
    cout << "num_terminal_edges: " << terminal_edges.size() << endl;
    cout << "num_terminal_non_edges: " << terminal_non_edges.size() << endl;
  }

  int num_sample_edges = terminal_edges.size() * ratio;
  // Sample nonterminal edges first, since this is much easier.
  assert(num_sample_edges <= terminal_non_edges.size());
  random_shuffle(terminal_non_edges.begin(), terminal_non_edges.end());
  for (int i = 0; i < num_sample_edges; i++) {
    auto edge = terminal_non_edges[i];
    sampled_edges[edge] = 0;  // 0 = not present in network
  }

  random_shuffle(terminal_edges.begin(), terminal_edges.end());
  for (int i = 0; i < num_sample_edges; i++) {
    auto edge = terminal_edges[i];
    if (spanning_tree.count(edge)) continue;
    sampled_edges[edge] = 1;  // 1 = present in the network
    int u = edge.first;
    int v = edge.second;
    assert(adj[u].count(v));
    assert(adj[v].count(u));
    adj[u].erase(v);
    adj[v].erase(u);
  }
}

bool IsGraphConnected() {
  UnionFind uf(n);
  for (int u = 0; u < adj.size(); u++) {
    for (auto edge : adj[u]) {
      int v = edge.first;
      uf.unionSet(u, v);
    }
  }
  return uf.numDisjointSets() == 1;
}

// -----------------------------------------------------------------------------
// Output
// -----------------------------------------------------------------------------

void WriteGraph(string filename, bool debug=false) {
  reduced_n = 0;
  reduced_m = 0;
  reduced_node_indexer.clear();
  ofstream fout(filename);
  map<ii, double> edges;
  for (int i = 0; i < n; i++) {
    if (adj[i].size() > 0) {  // hasn't been contracted
      for (auto edge : adj[i]) {
        int u = i;
        int v = edge.first;
        double w = edge.second;
        if (u >= v) continue;
        if (!reduced_node_indexer.count(u)) reduced_node_indexer[u] = reduced_n++;
        if (!reduced_node_indexer.count(v)) reduced_node_indexer[v] = reduced_n++;
        edges[ii(u, v)] = w;
      }
    }
  }
  reduced_m = edges.size();
  fout << reduced_n << " " << reduced_n << " " << reduced_m << endl;
  for (auto edge : edges) {
    int u = reduced_node_indexer[edge.first.first];
    int v = reduced_node_indexer[edge.first.second];
    double w = edge.second;
    fout << u << " " << v << " " << w << endl;
  }
}

void WriteLabels(string filename, bool debug=false) {
  ofstream fout(filename);
  reduced_label_indexer.clear();
  int num_label_types = 0;
  int num_output_lines = 0;
  for (const auto& kv : node_to_label) {
    if (eliminated_set.count(kv.first)) continue;
    for (auto label : kv.second) {
      if (!reduced_label_indexer.count(label)) {
        reduced_label_indexer[label] = num_label_types++;
      }
      num_output_lines++;
    }
  }
  fout << reduced_n << " " << num_label_types << " ";
  fout << num_output_lines << endl;
  for (const auto& kv : node_to_label) {
    if (eliminated_set.count(kv.first)) continue;
    for (auto label : kv.second) {
      fout << reduced_node_indexer[kv.first] << " ";
      fout << reduced_label_indexer[label] << endl;
    }
  }
}

void WriteTrainingSet(string filename, bool debug=false) {
  ofstream fout(filename);
  fout << "num_labeled_edges, num_nodes, num_categories" << endl;
  fout << sampled_edges.size() << " " << n << " " << 2 << endl;
  for (auto kv : sampled_edges) {
    int u = reduced_node_indexer.at(kv.first.first);
    int v = reduced_node_indexer.at(kv.first.second);
    if (u > v) swap(u, v);
    fout << u << " " << v << " " << kv.second << endl;
  }
}

int main() {
  const int SEED = 0;
  srand(SEED);
  bool debug = false;

  const string input_graph = "ppi.network.txt";
  ReadGraph(input_graph);
  const string labels_filename = "ppi_2000.group.txt";
  ReadLabels(labels_filename);

  // Edges in spanning tree are not removed to maintain connectivity.
  FindSpanningTree();

  SampleTerminalAndNonterminalEdges(0.5);
  cout << "num_training_examples: " << sampled_edges.size() << endl;
  assert(IsGraphConnected());

  const string output_graph = "ppi_2000_lp.network.txt";
  WriteGraph(output_graph);
  const string output_labels = "ppi_2000_lp.group.txt";
  WriteLabels(output_labels);
  const string output_training_set = "ppi_2000_lp.training_set.txt";
  WriteTrainingSet(output_training_set);

  return 0;
}
