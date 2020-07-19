import numpy as np
import scipy.sparse as sparse
import scipy.io
import logging
import sys

#graph_file = 'facebook_500_lp.sc.network.txt'
#label_file = 'facebook_500_lp.sc.group.txt'
#outputfile = 'facebook_500_lp.sc.mat'

def main():
    if len(sys.argv) != 4:
        print('Usage: python3 txt2mat.py network_file label_file output_file')
        return

    graph_file = sys.argv[1]
    label_file = sys.argv[2]
    output_file = sys.argv[3]

    print("Reading graph from:", graph_file)
    n = int(np.loadtxt(graph_file, max_rows=1)[0])
    print("Number of nodes:", n)
    data = np.loadtxt(graph_file, skiprows=1)
    X = sparse.csr_matrix((np.concatenate((data[:,2],)*2),(np.concatenate((data[:,0],data[:,1])),np.concatenate((data[:,1],data[:,0])))),shape=(n,n))
    print("X shape:", X.shape)

    print("Reading labels from:", label_file)    
    n, k, nnz = map(int, np.loadtxt(label_file, max_rows=1))  # dimensions of label matrix
    print("Number of labeled nodes, classes, and nonzeros:", n, k, nnz)
    data = np.loadtxt(label_file, skiprows=1)
    Y = sparse.csr_matrix((np.ones(nnz), (data[:,0], data[:,1])), shape=(n, k))
    print("Y shape:", Y.shape)

    scipy.io.savemat(output_file, mdict={'network': X, 'group': Y})

if __name__ == "__main__":
    main()

