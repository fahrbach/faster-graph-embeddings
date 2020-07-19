/*
 * Schur complement onto subset of vertices with labels
 * 
 * Fileformats are in the header of the input / output functions
 * 	ReadGraph()
 * 	ReadKeep();
 *
 * */

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <iostream>
#include <cassert>
#include <queue>
using namespace std;

#define MP make_pair 
#define A first 
#define B second 

#define PB push_back 
#define FR(i, a, b) for(int i=(a); i<(b); i++) 
#define FOR(i, n) FR(i, 0, n) 

const int MAXV = 2000100;

map<int, double> e[MAXV];
double weighted_degree[MAXV], slack_degree[MAXV];
int n, m;
int keep[MAXV], keep_n;
int removed[MAXV];

char line[1234];

void ReadGraph(string fname) {
	//format: matrix represented as edge list,
	// line 1 first number: # vertices
	// til end of file: one edge per line, two end points,
	//   and possibly edges (or not)
	FILE* f_in = fopen(fname.c_str(), "r");
	fgets(line, 1000, f_in);
	sscanf(line, "%d", &n);
	int a, b;
	double w;
	while (fgets(line, 1000, f_in)) {
		//attempt at auto-switching between weighted and unweighted
		if(sscanf(line, "%d%d%lf", &a, &b, &w) != 3) {
			assert(sscanf(line, "%d%d", &a, &b) == 2);
			w = 1;
		}
    if (a == b) continue;
		e[a][b] = w;
		e[b][a] = w;
	}
	fclose(f_in);
}

int n_label;
vector<pair<int, int> > labels;

void ReadLabels(string fname) {
	//format: 
	//	 list of numbers that's designed to be also compatible with labels file
	// line 1 ignore
	// til end of file: first number per line is the vertex to keep
	FILE* f_in = fopen(fname.c_str(), "r");
	printf("Reading labels from %s\n", fname.c_str());
	fgets(line, 1000, f_in);
	int n1, m1;
	sscanf(line, "%d%d%d", &n1, &n_label, &m1);
	assert(n1 == n);
	int a, b;
	while (fscanf(f_in, "%d%d", &a, &b) == 2) {
		labels.PB(MP(a, b));
		keep[a] = 1;
	}
	fclose(f_in);

	keep_n = 0;
	FOR(u, n) {
		keep_n += keep[u];
	}
}

int heap[MAXV], heap_reverse[MAXV], heap_n;

void HeapSwap(int p1, int p2) {
	swap(heap[p1], heap[p2]);
	heap_reverse[heap[p1]] = p1;
	heap_reverse[heap[p2]] = p2;
}

inline int Better(int p1, int p2) {
	return MP(e[p1].size(), p1) < MP(e[p2].size(), p2);
}

void HeapUp(int p) {
	assert(p <= heap_n);
	while (p != 1 && Better(heap[p], heap[p / 2])) {
		HeapSwap(p, p / 2);
		p /= 2;
	}
}

void HeapDown(int p) {
	assert(p <= heap_n);
	while (p * 2 <= heap_n) {
		int p1 = p * 2;
		if (p * 2 + 1 <= heap_n && Better(heap[p * 2 + 1], heap[p * 2])) {
			p1 = p * 2 + 1;
		}
		if (!Better(heap[p1], heap[p])) {
			break;
		}
		HeapSwap(p, p1);
		p = p1;
	}
}

void HeapUpdate(int p) {
	assert(p <= heap_n);
	HeapUp(p);
	HeapDown(p);
}

int n_filtered, m_new;
vector<int> from, to;

void Aggregate() {
	m_new = 0;
	FOR(u, n) {
		to.PB(-1);
		if (!removed[u]) {
			from.PB(u);
			to[u] = from.size() - 1;
			m_new += e[u].size();
		}
	}
	n_filtered = from.size();
	m_new /= 2;
}

void PrintRaw(string out_file) {
	int m1 = 0;
	printf("outputting raw Schur Complement on same vertex set to %s\n", out_file.c_str());
	FILE* f_out_graph = fopen(out_file.c_str(), "w");
	fprintf(f_out_graph, "%d %d\n", n, m_new);
	FOR(u, n) if (!removed[u]) {
		for (auto e1 : e[u]) if (e1.A > u) {
			fprintf(f_out_graph, "%d %d %0.3g\n", u, e1.A, e1.B);
			m1++;
		}
	}
	printf("%d edges outputted\n", m1);
}

void PrintMapping(string mapping_file) {
	printf("outputting mapping of vertices to %s\n", mapping_file.c_str());
	FILE *f_out = fopen(mapping_file.c_str(), "w");
	fprintf(f_out, "# new_node_id, original_node_id\n");
	fprintf(f_out, "# %d %d\n", n_filtered, n);
	FOR(i, n_filtered) {
		fprintf(f_out, "%d %d\n", i, from[i]);
	}
	fclose(f_out);
}

void PrintGraph(string out_file) {
	printf("outputting Schur Complement to %s\n", out_file.c_str());
	FILE* f_out = fopen(out_file.c_str(), "w");
	fprintf(f_out, "%d %d %d\n", n_filtered, n_filtered, m_new);
	int m1 = 0;
	FOR(u, n) if (to[u] != -1) {
		for (auto e1 : e[u]) if (e1.A > u) {
			fprintf(f_out, "%d %d %0.3g\n", to[e1.A], to[u], e1.B);
			m1++;
		}
	}
	fclose(f_out);
	printf("%d edges outputted\n", m1);
}

void PrintLabels(string out_file) {
	printf("outputting labels to %s\n", out_file.c_str());
	FILE *f_out = fopen(out_file.c_str(), "w");
	fprintf(f_out, "%d %d %d\n", n_filtered, n_label, labels.size());
	for(auto x:labels) {
		fprintf(f_out, "%d %d\n", to[x.A], x.B);
	}
	fclose(f_out);
}

void Fill(int s) {
	vector<int> vis(n);
	FOR(u, n) vis[u] = 0;
	queue<int> Q;
	Q.push(s);
	vis[s] = 1;
	while(!Q.empty()) {
		int u = Q.front();
		Q.pop();
		for(auto e1:e[u]) if(!vis[e1.A]){
			vis[e1.A] = 1;
			Q.push(e1.A);
		}
	}

	//discard everything not in the giant component
	int n_removed = 0;
	FOR(u, n) if(!vis[u]) {
		n_removed++;
		removed[u] = 1;
		keep[u] = 0;
		e[u].clear();
	}
	printf("%d vertices removed because not connected with vertex %d\n", n_removed, s);
}

int main(int argt, char **args) {
	puts("USAGE: 4 parameters");
	puts("         parameter 1: graph file");
	puts("         parameter 2: labels file");
	puts("         parameter 3: theta (1 - PageRank reset probability, NetMF does 0.9)");
	puts("         parameter 4: degree threshold");
	assert(argt == 5);

	string graph_file_name = string(args[1]);
	printf("Working on file %s\n", graph_file_name.c_str());
	string labels_file_name = string(args[2]);
	printf("Working on file %s\n", labels_file_name.c_str());

  double theta;
	sscanf(args[3], "%lf", &theta);
	printf("PageRank reset probability (theta) = %lf\n", theta);

	int degree_threshold;
	sscanf(args[4], "%d", &degree_threshold);
	
	ReadGraph(graph_file_name);
	FOR(u, n) {
		keep[u] = 0;
		removed[u] = 0;
    // Normal notion of weighted graph
		weighted_degree[u] = 0;
		for (auto e1 : e[u]) {
			weighted_degree[u] += e1.B;
		}
    // Now apply theta so that the graph corresponds to D - theta*A (with slack)
    for (auto& e1 : e[u]) {
      e1.B *= theta;
    }
    slack_degree[u] = (1 - theta) * weighted_degree[u];
    weighted_degree[u] *= theta;
	}
	ReadLabels(labels_file_name);

	//only eliminate things on the giant component
	Fill(0);

	heap_n = 0;
	FOR(u, n) if (!keep[u] & !removed[u]) {
		heap_n++;
		heap[heap_n] = u;
		heap_reverse[u] = heap_n;
		HeapUp(heap_n);
	} else {
		heap_reverse[u] = -1;
	}

//FOR(u, n) if(keep[u] && e[u].size() == 0) printf("...%d\n", u);

	while (heap_n > 0 && e[heap[1]].size() <= degree_threshold) {
		int u = heap[1];
//if(e[u].size() == 0) printf("%d\n", u);
		printf("pivoting %d(%d), degree = %d\n", u, heap_reverse[u], e[u].size());
		assert(!removed[u]);

		removed[u] = 1;
		HeapSwap(1, heap_n);
		heap_n--;
		HeapDown(1);
		//printf("%d %d\n", heap_reverse[549396], heap_n);
		//if(u == 628780) { puts("hi");  fflush(stdout); for(auto e1:e[u]) printf("u-%d %lf\n", e1.A, e1.B);}
		for (auto e1 : e[u]) {
			for (auto e2 : e[u]) if (e2.A > e1.A) {
				double w = e1.B * e2.B / (weighted_degree[u] + slack_degree[u]);
				e[e1.A][e2.A] += w;
				e[e2.A][e1.A] += w;
				//if(u == 628780) { printf("%d %d %lf\n", e1.A, e2.A, w); fflush(stdout);}
				weighted_degree[e1.A] += w;
				weighted_degree[e2.A] += w;
			}
      // New Schur complement update
      slack_degree[e1.A] += (e1.B * slack_degree[u]) / (weighted_degree[u] + slack_degree[u]);
		}
		//if(u == 628780) { puts("hi");  fflush(stdout);}
		for (auto e1 : e[u]) {
			weighted_degree[e1.A] -= e1.B;
			e[e1.A].erase(u);
		}
		for (auto e1 : e[u]) {
			assert(!removed[e1.A]);
			//printf("%d\n", e1.A);
			HeapUpdate(heap_reverse[e1.A]);
		}
	}
//int u = 635569;
//printf("%d (%d) %d %d\n", u, keep[u], e[u].size(), heap_reverse[u]);
	FOR(u, n) if(!removed[u]) {
          assert(e[u].size() > 0);
          assert(e[u].find(u) == e[u].end());
        }

	Aggregate();
	printf("new graph on %d vertices, %d edges\n", n_filtered, m_new);
	PrintGraph("sc.network.txt");
	PrintMapping("sc.reverse.txt");
	PrintLabels("sc.group.txt");
	return 0;
}
