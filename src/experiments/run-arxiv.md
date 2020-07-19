# Astro Physics (arxiv) link prediction experiment (4000 nodes)

## Make arxiv network.txt file (from combined SNAP file, largest component, remove self loops).
g++ -O2 make_arxiv_network.cc -o make_arxiv_network && ./make_arxiv_network

## Label 1000 vertices as terminals (i.e., label = 0).
g++ -O2 make_random_subset_arxiv.cc -o make_random_subset_arxiv
./make_random_subset_arxiv < arxiv.network.txt > arxiv_subset_4000.txt

g++ -O2 make_group_file.cc -o make_group_file
./make_group_file arxiv.network.txt arxiv_subset_4000.txt arxiv_4000.group.txt

## Make graph without positive example edges (training set: arxiv_4000_lp.training_set.txt)
g++ -O2 make_labeled_dataset.cc -o make_labeled_dataset && ./make_labeled_dataset
python3 txt2mat.py arxiv_4000_lp.network.txt arxiv_4000_lp.group.txt arxiv_4000_lp.mat

## Get Random Contraction graph
g++ -O2 random-contraction.cc -o random-contraction
./random-contraction arxiv_4000_lp.network.txt arxiv_4000_lp.group.txt
python3 txt2mat.py arxiv_4000_lp.rc.network.txt arxiv_4000_lp.rc.group.txt arxiv_4000_lp.rc.mat

## Get Schur Complement graph
g++-9 -O2 -o schur-complement schur-complement.cc
./schur-complement arxiv_4000_lp.network.txt arxiv_4000_lp.group.txt 0.9 30
python3 txt2mat.py arxiv_4000_lp.sc.network.txt arxiv_4000_lp.sc.group.txt arxiv_4000_lp.sc.mat

## Compute LINE embeddings
python3 classification/Line.py --input data/arxiv_4000_lp.mat --output embeddings/arxiv_4000_lp.Line.embedding.npy
python3 classification/Line.py --input data/arxiv_4000_lp.rc.mat --output embeddings/arxiv_4000_lp.rc.Line.embedding.npy
python3 classification/Line.py --input data/arxiv_4000_lp.sc.mat --output embeddings/arxiv_4000_lp.sc.Line.embedding.npy

## Evaluate LINE embeddings
python3 classification/link-prediction.py --label data/arxiv_4000_lp.mat --embedding embeddings/arxiv_4000_lp.Line.embedding.npy --training_set data/arxiv_4000_lp.training_set.txt 
python3 classification/link-prediction.py --label data/arxiv_4000_lp.rc.mat --embedding embeddings/arxiv_4000_lp.rc.Line.embedding.npy --training_set data/arxiv_4000_lp.training_set.txt --reverse_map data/arxiv_4000_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/arxiv_4000_lp.sc.mat --embedding embeddings/arxiv_4000_lp.sc.Line.embedding.npy --training_set data/arxiv_4000_lp.training_set.txt --reverse_map data/arxiv_4000_lp.sc.reverse.txt

## Compute NetMF embeddings
python3 classification/NetMF.py --input data/arxiv_4000_lp.mat --output embeddings/arxiv_4000_lp.NetMF.embedding.npy
python3 classification/NetMF.py --input data/arxiv_4000_lp.rc.mat --output embeddings/arxiv_4000_lp.rc.NetMF.embedding.npy
python3 classification/NetMF.py --input data/arxiv_4000_lp.sc.mat --output embeddings/arxiv_4000_lp.sc.NetMF.embedding.npy

## Evaluate NetMF embeddings
python3 classification/link-prediction.py --label data/arxiv_4000_lp.mat --embedding embeddings/arxiv_4000_lp.NetMF.embedding.npy --training_set data/arxiv_4000_lp.training_set.txt 
python3 classification/link-prediction.py --label data/arxiv_4000_lp.rc.mat --embedding embeddings/arxiv_4000_lp.rc.NetMF.embedding.npy --training_set data/arxiv_4000_lp.training_set.txt --reverse_map data/arxiv_4000_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/arxiv_4000_lp.sc.mat --embedding embeddings/arxiv_4000_lp.sc.NetMF.embedding.npy --training_set data/arxiv_4000_lp.training_set.txt --reverse_map data/arxiv_4000_lp.sc.reverse.txt
