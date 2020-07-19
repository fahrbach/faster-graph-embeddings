# Faster Graph Embeddings via Coarsening

## Datasets
The three datasets we consider are widely used for the multiclass vertex
classification task, and .mat files of these networks and their labels can be
found at:
  - http://leitang.net/code/social-dimension/data/blogcatalog.mat
  - http://leitang.net/code/social-dimension/data/flickr.mat
  - http://leitang.net/code/social-dimension/data/youtube.mat

For each of these .mat files, we run the `mat2txt.py` program to create separate
files for the adjacency lists and their corresponding labels. For the YouTube
experiment, we only consider the largest connected component of the network. The
following files in this archive are the starting point of our experiments:
  - data/blogcatalog.group.txt
  - data/blogcatalog.network.txt
  - data/flickr.group.txt
  - data/flickr.network.txt
  - data/youtube_connected.group.txt
  - data/youtube_connected.network.txt

For the link prediction experiments, we build of the experiment framework in
node2vec and use three popular networks available in the SNAP dataset.

## Graph embedding and classification pipelines
We include the complete command history for our classification experiments in
three annotated Markdown files. This includes descriptions of the parameter
settings for the graph embedding and prediction tools:
  - experiments/run-blogcatalog.md
  - experiments/run-flickr.md
  - experiments/run-youtube.md

There are many embeddings per network, each of dimension d=128, so we do include
the embeddings in this archive. However, they can be computed reasonably
efficiently using the scripts above using a moderately powerful machine.

Instructions to run the link prediction experiments follow a similar pattern.

## File descriptions
The graph embedding and prediction pipelines (i.e., LINE, NetMF, and NetSMF) are
part of the following repositories:
  - github.com/xptree/NetMF
  - github.com/xptree/NetSMF

We include the following tools in our archive:
  - classification/Line.py        (LINE embedding algorithm)
  - classification/NetMF.py       (NetMF embedding algorithm)
  - classification/NetSMF/        (NetSMF embedding algorithm)
  - classification/prediction.py  (multiclass vertex classification pipeline)
  - data/random-contraction.cc    (RandomContraction preprocessing algorithm)
  - data/schur-complement.cc      (SchurComplement preprocessing algorithm)
  - data/mat2txt.py               (extracts network and labels from .mat file)
  - data/txt2mat.py               (creates .mat file from network and labels)
  - data/mat2edgelist.py          (creates edge list from .mat file of a graph)

The following three programs are used to reduce the number of labeled vertices
for the Blogcatalog and Flickr experiments, since all vertices are labeled in
the original dataset and we want to eliminate unlabeled vertices:
  - data/make_random_subset_blogcatalog.cc  (get random subset of labeled nodes)
  - data/make_random_subset_flickr.cc
  - data/downsample-labels.py               (create group file on sampled nodes)

The link prediction experiments are somewhat more streamlined and can be
understood from their instructions in the experiments directory.

## Prediction results
For each embedding and corresponding .mat file, the vertex classification
scores (both micro-F1 and macro-F1) are recorded in a .log file associated with
the graph embedding file. We plot the results of all these experiments using
the Python tools in the plots/ directory.
