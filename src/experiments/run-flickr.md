# Flickr multilabel vertex classification experiment

## Create original graph .mat file with downsampled labels (in data/ directory)
$ g++-9 -O2 make_random_subset_flickr.cc
$ ./a.out flickr.network.txt > flickr_subset_40000.txt 
$ python3 downsample-labels.py
$ python3 txt2mat.py flickr.network.txt flickr_subset_40000.group.txt flickr.mat
$ python3 mat2edgelist.py flickr.mat flickr.edgelist

## Create random contraction .mat file
$ g++-9 -O2 -o random-contraction random-contraction.cc
$ time ./random-contraction
$ mv rc.network.txt flickr.rc.network.txt
$ mv rc.group.txt flickr.rc.group.txt
$ python3 txt2mat.py flickr.rc.network.txt flickr.rc.group.txt flickr.rc.mat

## Create Schur complement .mat file
$ g++-9 -O2 -o schur-complement schur-complement.cc
$ time ./schur-complement flickr.network.txt flickr_subset_40000.group.txt 0.9 30
$ mv sc1.network.txt flickr.sc.network.txt
$ mv sc1.group.txt flickr.sc.group.txt
$ python3 txt2mat.py flickr.sc.network.txt flickr.sc.group.txt flickr.sc.mat

## Compute LINE embeddings
$ time python3 classification/Line.py --input data/flickr.mat --ouput embeddings/flickr.Line.embedding.npy 
$ time python3 classification/Line.py --input data/flickr.rc.mat --ouput embeddings/flickr.rc.Line.embedding.npy 
$ time python3 classification/Line.py --input data/flickr.sc.mat --ouput embeddings/flickr.sc.Line.embedding.npy 

## Compute NetMF embeddings
$ time python3 classification/NetMF.py --input data/flickr.mat --ouput embeddings/flickr.NetMF.embedding.npy 
$ time python3 classification/NetMF.py --input data/flickr.rc.mat --ouput embeddings/flickr.rc.NetMF.embedding.npy 
$ time python3 classification/NetMF.py --input data/flickr.sc.mat --ouput embeddings/flickr.sc.NetMF.embedding.npy 

## Compute NetSMF embeddings
$ time python3 classification/NetSMF/bin/Release/netsmf -filename data/flickr.edgelist -machine $HOSTNAME -output_svd embeddings/flickr.netsmf -rank 512 -num_threads_sampling 100 -num_threads_svd 100 -rounds 300 -check_point 10 -weight -nolog1p -log4cxx NetSMF/example/log4cxx.config
$ time python3 classification/NetSMF/example/redsvd2emb.py --name embeddings/flickr.netsmf --dim 128

## Perform multilabel vertex classification task on embeddings of original graph
$ python3 classification/predict.py --label data/flickr.mat --embedding embeddings/flickr.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/flickr.mat --embedding embeddings/flickr.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/flickr.mat --embedding embeddings/flickr.netsmf.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10

## Perform multilabel vertex classification task on random contraction embeddings
$ python3 classification/predict.py --label data/flickr.rc.mat --embedding embeddings/flickr.rc.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/flickr.rc.mat --embedding embeddings/flickr.rc.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10

## Perform multilabel vertex classification task on Schur complement embeddings
$ python3 classification/predict.py --label data/flickr.sc.mat --embedding embeddings/flickr.sc.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/flickr.sc.mat --embedding embeddings/flickr.sc.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
