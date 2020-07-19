import scipy.io
import scipy.sparse as sparse
from scipy.sparse import csgraph
from scipy.linalg import inv
import numpy as np
import argparse
import logging
import theano
#from theano import tensor as T

logger = logging.getLogger(__name__)
#theano.config.exception_verbosity='high'

def load_adjacency_matrix_txt(file):
    n = int(np.loadtxt(file,max_rows=1)[0])
    print(n)
    data = np.loadtxt(file,skiprows=1)
    X=sparse.csr_matrix((np.concatenate((data[:,2],)*2),(np.concatenate((data[:,0],data[:,1])),np.concatenate((data[:,1],data[:,0])))),shape=(n,n))
    #data = scipy.io.loadmat(file)
    print(X[int(data[1,0]),int(data[1,1])],X[int(data[1,1]),int(data[1,0])])
    logger.info("saving mat file %s", file+'.mat')
    scipy.io.savemat(file+'.mat', mdict={'network': X})
    return X

def load_adjacency_matrix(file, variable_name="network"):
    data = scipy.io.loadmat(file)
    logger.info("loading mat file %s", file)
    return data[variable_name]

def deepwalk_filter(evals, window):
    for i in range(len(evals)):
        x = evals[i]
        evals[i] = 1. if x >= 1 else x*(1-x**window) / (1-x) / window
    evals = np.maximum(evals, 0)
    logger.info("After filtering, max eigenvalue=%f, min eigenvalue=%f",
            np.max(evals), np.min(evals))
    return evals

def approximate_normalized_graph_laplacian(A, rank, which="LA"):
    n = A.shape[0]
    #print(A[84997, 85013],A[85013,84997])
    #print(np.min(np.sum(A,axis=0)),np.min(np.sum(A,axis=1)))
    print(A.shape, A.count_nonzero())
    L, d_rt = csgraph.laplacian(A, normed=True, return_diag=True)
    #print(np.min(d_rt))
    # X = D^{-1/2} W D^{-1/2}
    X = sparse.identity(n) - L
    logger.info("Eigen decomposition...")
    #evals, evecs = sparse.linalg.eigsh(X, rank,
    #        which=which, tol=1e-3, maxiter=300)
    evals, evecs = sparse.linalg.eigsh(X, rank, which=which)
    #sparse.linalg.svds(X, rank, return_singular_vectors="u")
    logger.info("Maximum eigenvalue %f, minimum eigenvalue %f", np.max(evals), np.min(evals))
    logger.info("Computing D^{-1/2}U..")
    D_rt_inv = sparse.diags(d_rt ** -1)
    D_rt_invU = D_rt_inv.dot(evecs)
    return evals, D_rt_invU

def approximate_deepwalk_matrix(evals, D_rt_invU, window, vol, b):
    evals = deepwalk_filter(evals, window=window)
    X = sparse.diags(np.sqrt(evals)).dot(D_rt_invU.T).T
    #m = T.matrix()
    #mmT = T.dot(m, m.T) * (vol/b)
    Y = X.dot(X.T)*(vol/b)
    Y.data=np.log(np.maximum(1,Y.data))
    logger.info("Computed DeepWalk matrix with %d non-zero elements", np.count_nonzero(Y))
    return Y

def svd_deepwalk_matrix(X, dim):
    u, s, v = sparse.linalg.svds(X, dim, return_singular_vectors="u")
    # return U \Sigma^{1/2}
    return sparse.diags(np.sqrt(s)).dot(u.T).T

def evd_deepwalk_matrix(X, dim):
    evals, evecs = sparse.linalg.eigsh(X, dim, which='LA')
    # return U \Sigma^{1/2}
    return sparse.diags(np.sqrt(evals)).dot(evecs.T).T


def netmf_large(args):
    logger.info("Running NetMF for a large window size...")
    logger.info("Window size is set to be %d", args.window)
    # load adjacency matrix
    A = load_adjacency_matrix(args.input,
            variable_name=args.matfile_variable_name)
    vol = float(A.sum())
    # perform eigen-decomposition of D^{-1/2} A D^{-1/2}
    # keep top #rank eigenpairs
    evals, D_rt_invU = approximate_normalized_graph_laplacian(A, args.rank, which="LA")

    # approximate deepwalk matrix
    deepwalk_matrix = approximate_deepwalk_matrix(evals, D_rt_invU,
            window=args.window,
            vol=vol, b=args.negative)

    # factorize deepwalk matrix with SVD
    #logger.info("running SVD")
    #deepwalk_embedding = svd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("running EVD")
    deepwalk_embedding = evd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)

    logger.info("Save embedding to %s", args.output)
    np.save(args.output, deepwalk_embedding, allow_pickle=False)

def netmf_large_sparse(args):
    logger.info("Running NetMF for a large window size...")
    logger.info("Window size is set to be %d", args.window)
    # load adjacency matrix
    A = load_adjacency_matrix(args.input,
            variable_name=args.matfile_variable_name)
    vol = float(A.sum())
    # perform eigen-decomposition of D^{-1/2} A D^{-1/2}
    # keep top #rank eigenpairs
    evals, D_rt_invU = approximate_normalized_graph_laplacian(A, args.rank, which="LA")

    # approximate deepwalk matrix
    nnz = A.count_nonzero() * 50
    deepwalk_matrix = approximate_deepwalk_matrix_sparse(evals, D_rt_invU, nnz,
            window=args.window, 
            vol=vol, b=args.negative)

    # factorize deepwalk matrix with SVD
    #logger.info("running SVD")
    #deepwalk_embedding = svd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("running EVD")
    deepwalk_embedding = evd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)

    logger.info("Save embedding to %s", args.output)
    np.save(args.output, deepwalk_embedding, allow_pickle=False)


def direct_compute_deepwalk_matrix(A, window, b):
    n = A.shape[0]
    vol = float(A.sum())
    L, d_rt = csgraph.laplacian(A, normed=True, return_diag=True)
    # X = D^{-1/2} A D^{-1/2}
    X = sparse.identity(n) - L
    S = np.zeros_like(X)
    X_power = sparse.identity(n)
    for i in range(window):
        logger.info("Compute matrix %d-th power", i+1)
        X_power = X_power.dot(X)
        S += X_power
    S *= vol / window / b
    D_rt_inv = sparse.diags(d_rt ** -1)
    M = D_rt_inv.dot(D_rt_inv.dot(S).T)

    M.data=np.log(np.maximum(1,M.data))
    return M
    #m = T.matrix()
    #f = theano.function([m], T.log(T.maximum(m, 1)))
    #Y = f(M.todense().astype(theano.config.floatX))

    #return sparse.csr_matrix(Y)

def netmf_small(args):
    logger.info("Running NetMF for a small window size...")
    logger.info("Window size is set to be %d", args.window)
    # load adjacency matrix
    A = load_adjacency_matrix(args.input,
            variable_name=args.matfile_variable_name)
    # directly compute deepwalk matrix
    deepwalk_matrix = direct_compute_deepwalk_matrix(A,
            window=args.window, b=args.negative)

    # factorize deepwalk matrix with SVD
    #deepwalk_embedding = svd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("running EVD")
    deepwalk_embedding = evd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("Save embedding to %s", args.output)
    np.save(args.output, deepwalk_embedding, allow_pickle=False)

def line(args):
    logger.info("Running 1-step random walk...")
    #logger.info("Window size is set to be %d", args.window)
    # load adjacency matrix
    A = load_adjacency_matrix(args.input,
            variable_name=args.matfile_variable_name)
    # directly compute deepwalk matrix
    nnz = A.count_nonzero() * 10
    deepwalk_matrix = line_matrix(A,nnz)
    logger.info("Running SVD...")
    # factorize deepwalk matrix with SVD
    deepwalk_embedding = svd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("Save embedding to %s", args.output)
    np.save(args.output, deepwalk_embedding, allow_pickle=False)

def line_matrix(A,nnz):
    n = A.shape[0]
    vol = float(A.sum())
    L, d_rt = csgraph.laplacian(A, normed=True, return_diag=True)
    # X = D^{-1/2} A D^{-1/2}
    X = sparse.identity(n) - L
    D_rt_inv = sparse.diags(d_rt ** -1)
    M = D_rt_inv.dot(D_rt_inv.dot(X).T)        
    #M.data = np.log(M.data/np.min(M.data))    
    M.data = np.maximum(0,np.log(M.data*vol))    
    return M


def triangle_count(args):
    logger.info("Running short random walk...")
    #logger.info("Window size is set to be %d", args.window)
    # load adjacency matrix
    A = load_adjacency_matrix(args.input,
            variable_name=args.matfile_variable_name)
    # directly compute deepwalk matrix
    nnz = A.count_nonzero() * 10
    deepwalk_matrix = triangle_matrix(A,nnz)
    logger.info("Running SVD...")
    # factorize deepwalk matrix with SVD
    deepwalk_embedding = svd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("Save embedding to %s", args.output)
    np.save(args.output, deepwalk_embedding, allow_pickle=False)

def triangle_matrix(A, nnz):
    n = A.shape[0]
    vol = float(A.sum())
    L, d_rt = csgraph.laplacian(A, normed=True, return_diag=True)
    # X = D^{-1/2} A D^{-1/2}
    X = sparse.identity(n) - L
    #X2 = X.dot(X)
    #X = X + X2 # + X.dot(X2)
    M = X.dot(X)
    D_rt_inv = sparse.diags(d_rt ** -1)
    M = D_rt_inv.dot(D_rt_inv.dot(M).T)    
    flat_indices = np.argpartition(-M.data, nnz-1)[:nnz]
    ind = M.nonzero()   
    Y = M.data[flat_indices]    
    M = sparse.csr_matrix((Y/np.min(Y),(ind[0][flat_indices],ind[1][flat_indices])),shape=(n,n))
    M = M+D_rt_inv.dot(D_rt_inv.dot(X).T)*vol   
    M.data = np.log(M.data)
    #Y = np.log(Y/np.min(Y))
    return M

def approximate_deepwalk_matrix_sparse(evals, D_rt_invU, nnz, window, vol, b):
    evals = deepwalk_filter(evals, window=window)
    X = sparse.diags(np.sqrt(evals)).dot(D_rt_invU.T).T
    Y = X.dot(X.T).flatten()
    Z = -Y/np.random.rand(*Y.shape)
    flat_indices = np.argpartition(Z, nnz-1)[:nnz]
    #print(Y.shape)
    #flat_indices = np.argpartition(-Y, nnz-1)[:nnz]
    n = X.shape[0]
    rows = flat_indices//n
    cols = flat_indices-rows*n
    Y = Y[flat_indices]    
    M = sparse.csr_matrix((np.log(Y/np.min(Y)),(rows,cols)),shape=(n,n))
    #Y.data=np.log(np.maximum(1,Y.data))
    logger.info("Computed Sparse DeepWalk matrix with %d non-zero elements", np.count_nonzero(Y))
    return M

def inverse(args):
    logger.info("Running inverse...")
    # load adjacency matrix
    A = load_adjacency_matrix(args.input,
            variable_name=args.matfile_variable_name)
    # directly compute deepwalk matrix
    nnz = A.count_nonzero() * 10
    deepwalk_matrix = inverse_matrix(A,nnz)
    logger.info("Running EVD...")
    # factorize deepwalk matrix with SVD
    deepwalk_embedding = evd_deepwalk_matrix(deepwalk_matrix, dim=args.dim)
    logger.info("Save embedding to %s", args.output)
    np.save(args.output, deepwalk_embedding, allow_pickle=False)

def inverse_matrix(A,nnz):
    n = A.shape[0]
    #vol = float(A.sum())
    #L, d_rt = csgraph.laplacian(A, normed=True, return_diag=True)
    # X = D^{-1/2} A D^{-1/2}
    #X = sparse.identity(n) - L
    #D_rt_inv = sparse.diags(d_rt ** -1)
    #M = D_rt_inv.dot(D_rt_inv.dot(X).T) 
    diag_deg, _ = np.histogram(A.nonzero()[0], np.arange(A.shape[0]+1))       
    diag_mat = sparse.coo_matrix((n, n))
    diag_mat.setdiag(diag_deg)
    M =  inv((diag_mat-0.9*A).todense()).flatten()
    flat_indices = np.argpartition(-M, nnz-1)[:nnz]
    rows = flat_indices//n
    cols = flat_indices-rows*n
    Y = M[flat_indices]    
    M = sparse.csr_matrix((np.log(Y/np.min(Y)),(rows,cols)),shape=(n,n))
    #M.data = np.log(M.data/np.min(M.data))    
    #M.data = np.maximum(0,np.log(M.data*vol))    
    return M

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=str, required=False,
            help=".mat input file path",default="data/blogcatalog.mat")
    parser.add_argument('--matfile-variable-name', default='network',
            help='variable name of adjacency matrix inside a .mat file.')
    parser.add_argument("--output", type=str, required=False,
            help="embedding output file path",default="temp.embedding")

    parser.add_argument("--rank", default=256, type=int,
            help="#eigenpairs used to approximate normalized graph laplacian.")
    parser.add_argument("--dim", default=128, type=int,
            help="dimension of embedding")
    parser.add_argument("--window", default=10,
            type=int, help="context window size")
    parser.add_argument("--negative", default=1.0, type=float,
            help="negative sampling")

    parser.add_argument('--large', dest="large", action="store_true",
            help="using netmf for large window size")
    parser.add_argument('--small', dest="large", action="store_false",
            help="using netmf for small window size", )
    parser.set_defaults(large=True)

    args = parser.parse_args()
    logging.basicConfig(level=logging.INFO,
            format='%(asctime)s %(message)s') # include timestamp

    if args.large:
        netmf_large(args)
    else:
        netmf_small(args)
