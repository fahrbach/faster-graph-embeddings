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
  }
}

void AdjustGraph() {
  for (int node = 0; node < n; node++) {
    for (auto &kv : adj[node]) {
      weighted_degree[node] += kv.second;
      kv.second *= THETA;
    }
    slack_degree[node] = (1 - THETA) * weighted_degree[node];
    weighted_degree[node] *= THETA;
  }
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

// -----------------------------------------------------------------------------
// Random Contractions 
// -----------------------------------------------------------------------------

int RandomNeighbor(int node) {
  double degree = 0;
  for (auto kv : adj[node]) degree += kv.second;
  double r = (double)rand()/RAND_MAX;
  double running_sum = 0;
  bool found = false;
  int ans = -1;
  for (auto kv : adj[node]) {
    running_sum += kv.second;
    ans = kv.first;
    if (r*degree <= running_sum) {
      found = true;
      break;
    }
  }
  assert(found);
  assert(ans != -1);
  return ans;
}

// Variables used for min-degree variant
set<ii> min_degree_set;
vi degree_set_lookup;  // not always insync with adj[node].size()

// NOTE(fahrbach): Updated to approximate Schur complement
void Contract(int u, int v) {
  // Check that this edge exists in the graph
  assert(u != v);
  assert(adj[u].count(v));
  assert(adj[v].count(u));
  // Store old edge weight
  double contracted_edge_weight = adj[u][v];
  // Contract edge (u, v) and keep vertex v
  cluster.unionSet(u, v);
  // Build new neighborhood of v
  map<int, double> new_neighborhood;
  for (auto kv : adj[u]) {
    if (kv.first == u || kv.first == v) continue;
    // Update: Reweight edges in neighborhood of u (contracted vertex)
    double this_edge_weight = kv.second;
    double numerator = contracted_edge_weight*this_edge_weight;
    double new_coeff = weighted_degree[u] / (weighted_degree[u] + slack_degree[u]);
    double denominator = contracted_edge_weight + this_edge_weight;
    assert(denominator != 0.0);
    new_neighborhood[kv.first] += numerator * new_coeff / denominator;

    slack_degree[kv.first] += this_edge_weight*slack_degree[u]/(weighted_degree[u] + slack_degree[u]);
  }
//printf("::%d\n", new_neighborhood.size());
/*
  for (auto kv : adj[v]) {
    if (kv.first == u || kv.first == v) continue;
    new_neighborhood[kv.first] += kv.second;
  }
*/
  // Delete edges on u
  for (auto kv : adj[u]) adj[kv.first].erase(u);
  adj[u].clear();

  // Delete edges on v
//  adj[v].clear();
//  for (auto kv : adj[v]) adj[kv.first].erase(v);

  // Rebuild neighborhood of v
printf("%d\n", new_neighborhood.size());
  for (auto kv : new_neighborhood) {
    int neighbor = kv.first;
    adj[v][neighbor] += kv.second;
    adj[neighbor][v] += kv.second;
    weighted_degree[v] += kv.second;
    weighted_degree[neighbor] += kv.second;
  }
}

void RandomContractions(int target_num_nodes, bool debug=false) {
  target_num_nodes = min(n, target_num_nodes);
  vi permuted_nodes(n);
  iota(permuted_nodes.begin(), permuted_nodes.end(), 0);
  random_shuffle(permuted_nodes.begin(), permuted_nodes.end());
  int eliminated_count = 0;
  for (int i = 0; i < n && n - eliminated_count > target_num_nodes; i++) {
    int node = permuted_nodes[i];
    if (node_to_label.count(node)) continue;
    if (adj[node].size() == 0) continue;  // Can't contract an isolated node
    int neighbor = RandomNeighbor(node);
    if (debug) {
      cout << (n - eliminated_count) - target_num_nodes;
      cout << " contracting " << node << " into " << neighbor << endl;
    }
    eliminated_count++;
    eliminated_set.insert(node);
    Contract(node, neighbor);
  }
}

void GreedyRandomContractions(int target_num_nodes, bool debug=false) {
  target_num_nodes = min(n, target_num_nodes);
  int eliminated_count = 0;
  // Initialize data structures for maintaining min-degree nodes
  min_degree_set.clear();
  degree_set_lookup = vi(n);
  for (int i = 0; i < n; i++) {
    int degree = adj[i].size();
    min_degree_set.insert({degree, i});
    degree_set_lookup[i] = degree;
  }
  for (int i = 0; i < n && n - eliminated_count > target_num_nodes; i++) {
    // Find current minimum degree node
    if (min_degree_set.size() == 0) break;
    int node = (*min_degree_set.begin()).second;
    assert(min_degree_set.find({degree_set_lookup[node], node}) != min_degree_set.end());
    min_degree_set.erase({degree_set_lookup[node], node});
    if (node_to_label.count(node)) continue;
    // Trying target min degree
    if (adj[node].size() > TARGET_DEGREE) return;

//printf("%d\n", adj[node].size());
map<int, double> temp = adj[node];
    if (adj[node].size() == 0) continue;  // Can't contract an isolated node
    int neighbor = RandomNeighbor(node);
    if (debug) {
      cout << (n - eliminated_count) - target_num_nodes;
      cout << " contracting " << node << " into " << neighbor << " -- " << adj[node].size() << endl;
    }
    eliminated_count++;
    eliminated_set.insert(node);
    Contract(node, neighbor);
    // Update the neighbors of node in the min heap
    degree_set_lookup[node] = 0;
    if (!node_to_label.count(neighbor)) {
      assert(min_degree_set.find({degree_set_lookup[neighbor], neighbor}) != min_degree_set.end());
      min_degree_set.erase({degree_set_lookup[neighbor], neighbor});
      degree_set_lookup[neighbor] = adj[neighbor].size();
      min_degree_set.insert({degree_set_lookup[neighbor], neighbor});
    }
//printf("%d\n", adj[neighbor].size());
//    for (auto kv : adj[neighbor]) {
    for (auto kv : temp) {
      int v = kv.first;
      if (!node_to_label.count(v)) {
        assert(min_degree_set.find({degree_set_lookup[v], v}) != min_degree_set.end());
        min_degree_set.erase({degree_set_lookup[v], v});
        degree_set_lookup[v] = adj[v].size();
        min_degree_set.insert({degree_set_lookup[v], v});
      }
    }
  }
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

void WriteReverseMap(string filename, bool debug=false) {
  ofstream fout(filename);
  map<int, int> parent_to_surviving_node;
  fout << "# new_node_id, original_node_id" << endl;
  fout << "# num_nodes: " << reduced_node_indexer.size() << ", " << n << endl;
  map<int, int> rev;
  for (auto kv : reduced_node_indexer) {
    int original = kv.first;
    int new_node = kv.second;
    rev[new_node] = original;
  }
  for (auto kv : rev) {
    fout << kv.first << " " << kv.second << endl;
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Usage: ./random-contraction network_filename group_filename" << endl;
    return 1;
  }

  const int SEED = 0;
  srand(SEED);
  bool debug = false;

  // Read graph
  //const string graph_filename = "facebook_500_lp.network.txt";
  const string graph_filename = string(argv[1]);
  cout << "Reading graph: " << graph_filename << endl;
  ReadGraph(graph_filename, debug);
  AdjustGraph();

  // Read labels (need to read graph first)
  //const string labels_filename = "facebook_500_lp.group.txt";
  const string labels_filename = string(argv[2]);
  cout << "Reading labels: " << labels_filename << endl;
  ReadLabels(labels_filename, debug);

  // Perform random contractions
  const int target_num_nodes = -1;
  //RandomContractions(target_num_nodes, debug);
  GreedyRandomContractions(target_num_nodes, debug);

  // Write graph
  const string reduced_graph_filename = "rc.network.txt";
  cout << "Writing graph: " << reduced_graph_filename << endl;
  WriteGraph(reduced_graph_filename, debug);
  // Write labels
  const string reduced_labels_filename = "rc.group.txt";
  cout << "Writing labels: " << reduced_labels_filename << endl;
  WriteLabels(reduced_labels_filename, debug);
  // Write reverse map
  const string reverse_map_filename = "rc.reverse.txt";
  cout << "Writing reverse: " << reduced_labels_filename << endl;
  WriteReverseMap(reverse_map_filename, debug);
  return 0;
}
