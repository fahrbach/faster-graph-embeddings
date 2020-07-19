import argparse
import logging
import numpy as np
import scipy.sparse as sp
import scipy.io
from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score

logger = logging.getLogger(__name__)

def load_label(file, variable_name="group"):
    if file.endswith(".tsv") or file.endswith(".txt"):
        data = np.loadtxt(file).astype(np.int32)
        label = sp.csr_matrix(([1] * data.shape[0], (data[:, 0], data[:, 1])), dtype=np.bool_)
        sp.save_npz("label.npz", label)
        return label
    elif file.endswith(".npz"):
        return sp.load_npz(file)
    else:
        data = scipy.io.loadmat(file)
        logger.info("loading mat file %s", file)

        label = data[variable_name].tocsr().astype(np.bool_)
        return label

    label = data[variable_name].todense().astype(np.int32)
    label = np.array(label)
    return label

def read_training_set(training_set_input, reverse_map_filename=None):
    if reverse_map_filename != None:
        reverse_map = {}
        line_counter = 0
        with open(reverse_map_filename) as reverse_map_file:
            for line in reverse_map_file.readlines():
                line_counter += 1
                if line_counter > 2:
                    new_node_id, old_node_id = [int(x) for x in line.strip().split()]
                    reverse_map[old_node_id] = new_node_id

    labeled_edges = {}
    line_counter = 0
    with open(training_set_input) as fin:
        for line in fin.readlines():
            # Account for first two lines
            if line_counter > 1:
                u, v, label = [int(x) for x in line.strip().split()]
                if reverse_map_filename != None:
                    # TODO(fahrbach): remap nodes in RC and SC...
                    assert(u in reverse_map)
                    assert(v in reverse_map)
                    u = reverse_map[u]
                    v = reverse_map[v]
                labeled_edges[(u, v)] = label
            line_counter += 1
    return labeled_edges

def feature_matrix_average(labeled_edges, embedding):
    y = []
    X = []
    for edge in labeled_edges:
        label = labeled_edges[edge]
        y.append(label)
        u = edge[0]
        v = edge[1]
        new_row = (embedding[u] + embedding[v]) * 0.5
        X.append(new_row)
    X = np.array(X)
    y = np.array(y)
    return X, y

def feature_matrix_hadamard(labeled_edges, embedding):
    y = []
    X = []
    for edge in labeled_edges:
        label = labeled_edges[edge]
        y.append(label)
        u = edge[0]
        v = edge[1]
        new_row = np.multiply(embedding[u], embedding[v])
        X.append(new_row)
    X = np.array(X)
    y = np.array(y)
    return X, y

def feature_matrix_weighted_L1(labeled_edges, embedding):
    y = []
    X = []
    for edge in labeled_edges:
        label = labeled_edges[edge]
        y.append(label)
        u = edge[0]
        v = edge[1]
        new_row = np.abs(embedding[u] - embedding[v])
        X.append(new_row)
    X = np.array(X)
    y = np.array(y)
    return X, y

def feature_matrix_weighted_L2(labeled_edges, embedding):
    y = []
    X = []
    for edge in labeled_edges:
        label = labeled_edges[edge]
        y.append(label)
        u = edge[0]
        v = edge[1]
        tmp = np.abs(embedding[u] - embedding[v])
        new_row = np.multiply(tmp, tmp)
        X.append(new_row)
    X = np.array(X)
    y = np.array(y)
    return X, y

def run_classification_experiment(X, y, title=''):
    logger.info("experiment: " + title)
    print('experiment:', title)
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.4, random_state=0)
    model = LogisticRegression(solver='lbfgs')
    model.fit(X_train, y_train)
    accuracy = model.score(X_test, y_test)
    auc = roc_auc_score(y_test, model.predict_proba(X_test)[:,1])
    print('accuracy:', accuracy)
    print('auc:', auc)
    logger.info("accuracy: %f", accuracy)
    logger.info("auc: %f", auc)
    print()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--label", type=str, required=True,
            help="input file path for labels (.mat)")
    parser.add_argument("--embedding", type=str, required=True,
            help="input file path for embedding (.npy)")
    parser.add_argument("--matfile-variable-name", type=str, default='group',
            help='variable name of adjacency matrix inside a .mat file.')
    parser.add_argument("--training_set", type=str, required=True,
            help="input file path for training set.")
    parser.add_argument("--reverse_map", type=str, default=None,
            help="input file path for reverse map (from coarsened to original node ids).")
    args = parser.parse_args()
    logging.basicConfig(
        filename="%s.log" % args.embedding, filemode="a", # uncomment this to log to file
        level=logging.INFO,
        format='%(asctime)s %(message)s') # include timestamp

    # The labeled vertices are in the terminal set.
    logger.info("Loading label from %s...", args.label)
    label = load_label(file=args.label, variable_name=args.matfile_variable_name)
    logger.info("Label loaded!")

    # Read the embedding corresponding to this .mat file.
    logger.info("embedding=%s", args.embedding)
    embedding = np.load(args.embedding)
    logger.info("Network embedding loaded!")
    logger.info("Embedding has shape %d, %d", embedding.shape[0], embedding.shape[1])

    # Read the training set.
    labeled_edges = read_training_set(args.training_set, args.reverse_map)

    X, y = feature_matrix_average(labeled_edges, embedding)
    run_classification_experiment(X, y, 'average')

    X, y = feature_matrix_hadamard(labeled_edges, embedding)
    run_classification_experiment(X, y, 'hadamard')

    X, y = feature_matrix_weighted_L1(labeled_edges, embedding)
    run_classification_experiment(X, y, 'weighted-L1')

    X, y = feature_matrix_weighted_L2(labeled_edges, embedding)
    run_classification_experiment(X, y, 'weighted-L2')
