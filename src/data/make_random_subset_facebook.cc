#include <bits/stdc++.h>
using namespace std;

int main() {
  int target_size = 1000;

  srand(0);
  int n, k, nnz;
  cin >> n >> k >> nnz;
  set<int> nodes_present;
  map<int, set<int>> labels;
  for (int i = 0; i < nnz; i++) {
    int node, label;
    cin >> node >> label;
    nodes_present.insert(node);
    labels[node].insert(label);
  }
  target_size = min(target_size, (int)nodes_present.size());
  vector<int> nodes_present_vec;
  for (auto node : nodes_present) {
    nodes_present_vec.push_back(node);
  }
  random_shuffle(nodes_present_vec.begin(), nodes_present_vec.end());
  cout << target_size << " " << n << endl;
  for (int i = 0; i < target_size; i++) {
    cout << nodes_present_vec[i] << endl;
  }
}
