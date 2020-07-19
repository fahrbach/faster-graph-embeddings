# PPI link prediction experiment (2000 nodes)

## Make ppi network.txt file (from combined SNAP file, largest component, remove self loops).
g++ -O2 make_ppi_network.cc -o make_ppi_network && ./make_ppi_network

## Label 2000 vertices as terminals (i.e., label = 0).
g++ -O2 make_random_subset_ppi.cc -o make_random_subset_ppi
./make_random_subset_ppi < ppi.network.txt > ppi_subset_2000.txt

g++ -O2 make_group_file.cc -o make_group_file
./make_group_file ppi.network.txt ppi_subset_2000.txt ppi_2000.group.txt

## Make graph without positive example edges (training set: ppi_2000_lp.training_set.txt)
g++ -O2 make_labeled_dataset.cc -o make_labeled_dataset && ./make_labeled_dataset
python3 txt2mat.py ppi_2000_lp.network.txt ppi_2000_lp.group.txt ppi_2000_lp.mat

## Get Random Contraction graph
g++ -O2 random-contraction.cc -o random-contraction
./random-contraction ppi_2000_lp.network.txt ppi_2000_lp.group.txt
python3 txt2mat.py ppi_2000_lp.rc.network.txt ppi_2000_lp.rc.group.txt ppi_2000_lp.rc.mat

## Get Schur Complement graph
g++-9 -O2 -o schur-complement schur-complement.cc
./schur-complement ppi_2000_lp.network.txt ppi_2000_lp.group.txt 0.9 30
python3 txt2mat.py ppi_2000_lp.sc.network.txt ppi_2000_lp.sc.group.txt ppi_2000_lp.sc.mat

## Compute LINE embeddings
python3 classification/Line.py --input data/ppi_2000_lp.mat --output embeddings/ppi_2000_lp.Line.embedding.npy
python3 classification/Line.py --input data/ppi_2000_lp.rc.mat --output embeddings/ppi_2000_lp.rc.Line.embedding.npy
python3 classification/Line.py --input data/ppi_2000_lp.sc.mat --output embeddings/ppi_2000_lp.sc.Line.embedding.npy

## Evaluate LINE embeddings
python3 classification/link-prediction.py --label data/ppi_2000_lp.mat --embedding embeddings/ppi_2000_lp.Line.embedding.npy --training_set data/ppi_2000_lp.training_set.txt 
python3 classification/link-prediction.py --label data/ppi_2000_lp.rc.mat --embedding embeddings/ppi_2000_lp.rc.Line.embedding.npy --training_set data/ppi_2000_lp.training_set.txt --reverse_map data/ppi_2000_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/ppi_2000_lp.sc.mat --embedding embeddings/ppi_2000_lp.sc.Line.embedding.npy --training_set data/ppi_2000_lp.training_set.txt --reverse_map data/ppi_2000_lp.sc.reverse.txt

## Compute NetMF embeddings
python3 classification/NetMF.py --input data/ppi_2000_lp.mat --output embeddings/ppi_2000_lp.NetMF.embedding.npy
python3 classification/NetMF.py --input data/ppi_2000_lp.rc.mat --output embeddings/ppi_2000_lp.rc.NetMF.embedding.npy
python3 classification/NetMF.py --input data/ppi_2000_lp.sc.mat --output embeddings/ppi_2000_lp.sc.NetMF.embedding.npy

## Evaluate NetMF embeddings
python3 classification/link-prediction.py --label data/ppi_2000_lp.mat --embedding embeddings/ppi_2000_lp.NetMF.embedding.npy --training_set data/ppi_2000_lp.training_set.txt 
python3 classification/link-prediction.py --label data/ppi_2000_lp.rc.mat --embedding embeddings/ppi_2000_lp.rc.NetMF.embedding.npy --training_set data/ppi_2000_lp.training_set.txt --reverse_map data/ppi_2000_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/ppi_2000_lp.sc.mat --embedding embeddings/ppi_2000_lp.sc.NetMF.embedding.npy --training_set data/ppi_2000_lp.training_set.txt --reverse_map data/ppi_2000_lp.sc.reverse.txt
