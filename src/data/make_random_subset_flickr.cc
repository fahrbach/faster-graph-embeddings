#include <bits/stdc++.h>
using namespace std;

int main() {
  srand(0);
  int num_nodes = 80513;
  int target = 5000;
  vector<int> v(num_nodes);
  iota(v.begin(), v.end(), 0);
  random_shuffle(v.begin(), v.end());
  cout << target << " " << num_nodes << endl;
  for (int i = 0; i < target; i++) {
    cout << v[i] << endl;
  }
}
