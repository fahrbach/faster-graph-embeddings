# Hacking around using code from NetMF:

import scipy.io
import scipy.sparse as sparse
from scipy.sparse import csgraph
from scipy.linalg import inv
import numpy as np
import argparse
import logging
import theano

logger = logging.getLogger(__name__)

def load_adjacency_matrix(file, variable_name="network"):
    data = scipy.io.loadmat(file)
    logger.info("loading mat file %s", file)
    return data[variable_name]


def svd_deepwalk_matrix(X, dim):
    u, s, v = sparse.linalg.svds(X, dim, return_singular_vectors="u")
    # return U \Sigma^{1/2}
    return sparse.diags(np.sqrt(s)).dot(u.T).T


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=str, required=False,
            help=".mat input file path",default="data/blogcatalog.mat")
    parser.add_argument("--output", type=str, required=False,
            help="embedding output file path",default="temp.embedding")

    args = parser.parse_args()
    logging.basicConfig(level=logging.INFO,
            format='%(asctime)s %(message)s') #include timestamp

    A = load_adjacency_matrix(args.input, 'network')
    vol = float(A.sum())

    d = np.array((A.sum(axis = 0))).squeeze()
    D = sparse.diags(d)
    D_inv = sparse.diags(d ** -1)
    M = D_inv.dot(A).dot(D_inv)
    logger.info("done generating M")

    logger.info("M has entries in range [%lf, %lf]", min(M.data), max(M.data))
    nnz = M.count_nonzero()
    M.data = np.maximum(0, M.data)
    logger.info("taking truncated log, scaling factor = %f", np.sum(A))
    M.data = np.maximum(0, np.log(M.data * np.sum(A)))
    
    M = sparse.csr_matrix(M)
    logger.info("%d non-zeros left in M", M.count_nonzero())

    logger.info("running SVD")
    embedding = svd_deepwalk_matrix(M, 128);

    logger.info("Save embedding to temp file")
    np.save(args.output, embedding, allow_pickle=False)
