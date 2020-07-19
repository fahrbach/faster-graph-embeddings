# Youtube multilabel vertex classification experiment

## Create original graph .mat file (in data/ directory)
$ python3 txt2mat.py youtube_connected.network.txt youtube_connected.group.txt youtube.mat
$ python3 mat2edgelist.py youtube.mat youtube.edgelist

## Create random contraction .mat file
$ g++-9 -O2 -o random-contraction random-contraction.cc
$ time ./random-contraction
$ mv rc.network.txt youtube.rc.network.txt
$ mv rc.group.txt youtube.rc.group.txt
$ python3 txt2mat.py youtube.rc.network.txt youtube.rc.group.txt youtube.rc.mat

## Create Schur complement .mat file
$ g++-9 -O2 -o schur-complement schur-complement.cc
$ time ./schur-complement youtube.network.txt youtube.group.txt 0.9 30
$ mv sc1.network.txt youtube.sc.network.txt
$ mv sc1.group.txt youtube.sc.group.txt
$ python3 txt2mat.py youtube.sc.network.txt youtube.sc.group.txt youtube.sc.mat

## Compute LINE embeddings
$ time python3 classification/Line.py --input data/youtube.mat --ouput embeddings/youtube.Line.embedding.npy 
$ time python3 classification/Line.py --input data/youtube.rc.mat --ouput embeddings/youtube.rc.Line.embedding.npy 
$ time python3 classification/Line.py --input data/youtube.sc.mat --ouput embeddings/youtube.sc.Line.embedding.npy 

## Compute NetMF embeddings
$ time python3 classification/NetMF.py --input data/youtube.mat --ouput embeddings/youtube.NetMF.embedding.npy 
$ time python3 classification/NetMF.py --input data/youtube.rc.mat --ouput embeddings/youtube.rc.NetMF.embedding.npy 
$ time python3 classification/NetMF.py --input data/youtube.sc.mat --ouput embeddings/youtube.sc.NetMF.embedding.npy 

## Compute NetSMF embeddings
$ time python3 classification/NetSMF/bin/Release/netsmf -filename data/youtube.edgelist -machine $HOSTNAME -output_svd embeddings/youtube.netsmf -rank 512 -num_threads_sampling 100 -num_threads_svd 100 -rounds 300 -check_point 10 -weight -nolog1p -log4cxx NetSMF/example/log4cxx.config
$ time python3 classification/NetSMF/example/redsvd2emb.py --name embeddings/youtube.netsmf --dim 128

## Perform multilabel vertex classification task on embeddings of original graph
$ python3 classification/predict.py --label data/youtube.mat --embedding embeddings/youtube.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/youtube.mat --embedding embeddings/youtube.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10 --window 2
$ python3 classification/predict.py --label data/youtube.mat --embedding embeddings/youtube.netsmf.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10

## Perform multilabel vertex classification task on random contraction embeddings
$ python3 classification/predict.py --label data/youtube.rc.mat --embedding embeddings/youtube.rc.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/youtube.rc.mat --embedding embeddings/youtube.rc.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10

## Perform multilabel vertex classification task on Schur complement embeddings
$ python3 classification/predict.py --label data/youtube.sc.mat --embedding embeddings/youtube.sc.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
$ python3 classification/predict.py --label data/youtube.sc.mat --embedding embeddings/youtube.sc.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 1 --stop-train-ratio 10 --num-train-ratio 10
