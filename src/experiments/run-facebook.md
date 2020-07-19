# Facebook link prediction experiment (1000 nodes)

## Make Facebook network.txt file (from combined SNAP file).
g++ -O2 make_facebook_network.cc -o make_facebook_network && ./make_facebook_network

## Label 1000 vertices as terminals (i.e., label = 0).
g++ -O2 make_random_subset_facebook.cc -o make_random_subset_facebook
./make_random_subset_facebook < facebook.network.txt > facebook_subset_1000.txt

g++ -O2 make_group_file.cc -o make_group_file
./make_group_file facebook.network.txt facebook_subset_1000.txt facebook_1000.group.txt

## Make graph without positive example edges (training set: facebook_1000_lp.training_set.txt)
g++ -O2 make_labeled_dataset.cc -o make_labeled_dataset && ./make_labeled_dataset
python3 txt2mat.py facebook_1000_lp.network.txt facebook_1000_lp.group.txt facebook_1000_lp.mat

## Get Random Contraction graph
g++ -O2 random-contraction.cc -o random-contraction
./random-contraction facebook_1000_lp.network.txt facebook_1000_lp.group.txt
python3 txt2mat.py facebook_1000_lp.rc.network.txt facebook_1000_lp.rc.group.txt facebook_1000_lp.rc.mat

## Get Schur Complement graph
g++-9 -O2 -o schur-complement schur-complement.cc
./schur-complement facebook_1000_lp.network.txt facebook_1000_lp.group.txt 0.9 30
python3 txt2mat.py facebook_1000_lp.sc.network.txt facebook_1000_lp.sc.group.txt facebook_1000_lp.sc.mat

## Compute LINE embeddings
python3 classification/Line.py --input data/facebook_1000_lp.mat --output embeddings/facebook_1000_lp.Line.embedding.npy
python3 classification/Line.py --input data/facebook_1000_lp.rc.mat --output embeddings/facebook_1000_lp.rc.Line.embedding.npy
python3 classification/Line.py --input data/facebook_1000_lp.sc.mat --output embeddings/facebook_1000_lp.sc.Line.embedding.npy

## Evaluate LINE embeddings
python3 classification/link-prediction.py --label data/facebook_1000_lp.mat --embedding embeddings/facebook_1000_lp.Line.embedding.npy --training_set data/facebook_1000_lp.training_set.txt 
python3 classification/link-prediction.py --label data/facebook_1000_lp.rc.mat --embedding embeddings/facebook_1000_lp.rc.Line.embedding.npy --training_set data/facebook_1000_lp.training_set.txt --reverse_map data/facebook_1000_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/facebook_1000_lp.sc.mat --embedding embeddings/facebook_1000_lp.sc.Line.embedding.npy --training_set data/facebook_1000_lp.training_set.txt --reverse_map data/facebook_1000_lp.sc.reverse.txt

## Compute NetMF embeddings
python3 classification/NetMF.py --input data/facebook_1000_lp.mat --output embeddings/facebook_1000_lp.NetMF.embedding.npy
python3 classification/NetMF.py --input data/facebook_1000_lp.rc.mat --output embeddings/facebook_1000_lp.rc.NetMF.embedding.npy
python3 classification/NetMF.py --input data/facebook_1000_lp.sc.mat --output embeddings/facebook_1000_lp.sc.NetMF.embedding.npy

## Evaluate LINE embeddings
python3 classification/link-prediction.py --label data/facebook_1000_lp.mat --embedding embeddings/facebook_1000_lp.NetMF.embedding.npy --training_set data/facebook_1000_lp.training_set.txt 
python3 classification/link-prediction.py --label data/facebook_1000_lp.rc.mat --embedding embeddings/facebook_1000_lp.rc.NetMF.embedding.npy --training_set data/facebook_1000_lp.training_set.txt --reverse_map data/facebook_1000_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/facebook_1000_lp.sc.mat --embedding embeddings/facebook_1000_lp.sc.NetMF.embedding.npy --training_set data/facebook_1000_lp.training_set.txt --reverse_map data/facebook_1000_lp.sc.reverse.txt

# Facebook link prediction experiment (500 nodes, better contrast)

## Make Facebook network.txt file (from combined SNAP file).
g++ -O2 make_facebook_network.cc -o make_facebook_network && ./make_facebook_network

## Label 500 vertices as terminals (i.e., label = 0).
g++ -O2 make_random_subset_facebook.cc -o make_random_subset_facebook
./make_random_subset_facebook < facebook.network.txt > facebook_subset_500.txt

g++ -O2 make_group_file.cc -o make_group_file
./make_group_file facebook.network.txt facebook_subset_500.txt facebook_500.group.txt

## Make graph without positive example edges (training set: facebook_500_lp.training_set.txt)
g++ -O2 make_labeled_dataset.cc -o make_labeled_dataset && ./make_labeled_dataset
python3 txt2mat.py facebook_500_lp.network.txt facebook_500_lp.group.txt facebook_500_lp.mat

## Get Random Contraction graph

## Get Schur Complement graph
g++-9 -O2 -o schur-complement schur-complement.cc
./schur-complement facebook_500_lp.network.txt facebook_500_lp.group.txt 0.9 30
python3 txt2mat.py

## Compute LINE embeddings
python3 classification/Line.py --input data/facebook_500_lp.mat --output embeddings/facebook_500_lp.Line.embedding.npy
python3 classification/Line.py --input data/facebook_500_lp.rc.mat --output embeddings/facebook_500_lp.rc.Line.embedding.npy
python3 classification/Line.py --input data/facebook_500_lp.sc.mat --output embeddings/facebook_500_lp.sc.Line.embedding.npy

## Evaluate LINE embeddings
python3 classification/link-prediction.py --label data/facebook_500_lp.mat --embedding embeddings/facebook_500_lp.Line.embedding.npy --training_set data/facebook_500_lp.training_set.txt 
python3 classification/link-prediction.py --label data/facebook_500_lp.rc.mat --embedding embeddings/facebook_500_lp.rc.Line.embedding.npy --training_set data/facebook_500_lp.training_set.txt --reverse_map data/facebook_500_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/facebook_500_lp.sc.mat --embedding embeddings/facebook_500_lp.sc.Line.embedding.npy --training_set data/facebook_500_lp.training_set.txt --reverse_map data/facebook_500_lp.sc.reverse.txt

## Compute NetMF embeddings
python3 classification/NetMF.py --input data/facebook_500_lp.mat --output embeddings/facebook_500_lp.NetMF.embedding.npy
python3 classification/NetMF.py --input data/facebook_500_lp.rc.mat --output embeddings/facebook_500_lp.rc.NetMF.embedding.npy
python3 classification/NetMF.py --input data/facebook_500_lp.sc.mat --output embeddings/facebook_500_lp.sc.NetMF.embedding.npy

## Evaluate LINE embeddings
python3 classification/link-prediction.py --label data/facebook_500_lp.mat --embedding embeddings/facebook_500_lp.NetMF.embedding.npy --training_set data/facebook_500_lp.training_set.txt 
python3 classification/link-prediction.py --label data/facebook_500_lp.rc.mat --embedding embeddings/facebook_500_lp.rc.NetMF.embedding.npy --training_set data/facebook_500_lp.training_set.txt --reverse_map data/facebook_500_lp.rc.reverse.txt
python3 classification/link-prediction.py --label data/facebook_500_lp.sc.mat --embedding embeddings/facebook_500_lp.sc.NetMF.embedding.npy --training_set data/facebook_500_lp.training_set.txt --reverse_map data/facebook_500_lp.sc.reverse.txt
