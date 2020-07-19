# Blogcatalog multilabel vertex classification experiment

## Create original graph .mat file with downsampled labels (in data/ directory)
$ g++-9 -O2 make_random_subset_blogcatalog.cc
$ ./a.out blogcatalog.network.txt > blogcatalog_subset_5000.txt 
$ python3 downsample-labels.py
$ python3 txt2mat.py blogcatalog.network.txt blogcatalog_subset_5000.group.txt blogcatalog_subset_5000.mat
$ python3 mat2edgelist.py blogcatalog_subset_5000.mat blogcatalog_subset_5000.edgelist

## Create random contraction .mat file
$ g++-9 -O2 -o random-contraction random-contraction.cc
$ time ./random-contraction
$ mv rc.network.txt blogcatalog_subset_5000_grc_theta_90_deg_30.network.txt
$ mv rc.group.txt blogcatalog_subset_5000_grc_theta_90_deg_30.group.txt
$ python3 txt2mat.py blogcatalog_subset_5000_grc_theta_90_deg_30.network.txt blogcatalog_subset_5000_grc_theta_90_deg_30.group.txt blogcatalog_subset_5000_grc_theta_90_deg_30.mat

## Create Schur complement .mat file
$ g++-9 -O2 -o schur-complement schur-complement.cc
$ time ./schur-complement blogcatalog.network.txt blogcatalog_subset_5000.group.txt 0.9 30
$ mv sc1.network.txt blogcatalog_subset_5000_sc_theta_90_deg_30.network.txt
$ mv sc1.group.txt blogcatalog_subset_5000_sc_theta_90_deg_30.group.txt
$ python3 txt2mat.py blogcatalog_subset_5000_sc_theta_90_deg_30.network.txt blogcatalog_subset_5000_sc_theta_90_deg_30.group.txt blogcatalog_subset_5000_sc_theta_90_deg_30.mat

## Compute LINE embeddings
$ time python3 classification/Line.py --input data/blogcatalog_subset_5000.mat --ouput embeddings/blogcatalog_subset_5000.Line.embedding.npy 
$ time python3 classification/Line.py --input data/blogcatalog_subset_5000_grc_theta_90_deg_30.mat --ouput embeddings/blogcatalog_subset_5000_grc_theta_90_deg_30.Line.embedding.npy 
$ time python3 classification/Line.py --input data/blogcatalog_subset_5000_sc_theta_90_deg_30.mat --ouput embeddings/blogcatalog_subset_5000_sc_theta_90_deg_30.Line.embedding.npy 

## Compute NetMF embeddings
$ time python3 classification/NetMF.py --input data/blogcatalog_subset_5000.mat --ouput embeddings/blogcatalog_subset_5000.NetMF.embedding.npy 
$ time python3 classification/NetMF.py --input data/blogcatalog_subset_5000_grc_theta_90_deg_30.mat --ouput embeddings/blogcatalog_subset_5000_grc_theta_90_deg_30.NetMF.embedding.npy 
$ time python3 classification/NetMF.py --input data/blogcatalog_subset_5000_sc_theta_90_deg_30.mat --ouput embeddings/blogcatalog_subset_5000_sc_theta_90_deg_30.NetMF.embedding.npy 

## Compute NetSMF embeddings
$ time python3 classification/NetSMF/bin/Release/netsmf -filename data/blogcatalog_subset_5000.edgelist -machine $HOSTNAME -output_svd embeddings/blogcatalog_subset_5000.netsmf -rank 512 -num_threads_sampling 100 -num_threads_svd 100 -rounds 1000 -check_point 10 -weight -nolog1p -log4cxx NetSMF/example/log4cxx.config
$ time python3 classification/NetSMF/example/redsvd2emb.py --name blogcatalog_subset_5000.netsmf --dim 128

## Perform multilabel vertex classification task on embeddings of original graph
$ python3 classification/predict.py --label data/blogcatalog_subset_5000.mat --embedding embeddings/blogcatalog_subset_5000.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9
$ python3 classification/predict.py --label data/blogcatalog_subset_5000.mat --embedding embeddings/blogcatalog_subset_5000.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9
$ python3 classification/predict.py --label data/blogcatalog_subset_5000.mat --embedding embeddings/blogcatalog_subset_5000.netsmf.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9

## Perform multilabel vertex classification task on random contraction embeddings
$ python3 classification/predict.py --label data/blogcatalog_subset_5000_grc_theta_90_deg_30.mat --embedding embeddings/blogcatalog_subset_5000_grc_theta_90_deg_30.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9
$ python3 classification/predict.py --label data/blogcatalog_subset_5000_grc_theta_90_deg_30.mat --embedding embeddings/blogcatalog_subset_5000_grc_theta_90_deg_30.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9

## Perform multilabel vertex classification task on Schur complement embeddings
$ python3 classification/predict.py --label data/blogcatalog_subset_5000_sc_theta_90_deg_30.mat --embedding embeddings/blogcatalog_subset_5000_sc_theta_90_deg_30.Line.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9
$ python3 classification/predict.py --label data/blogcatalog_subset_5000_sc_theta_90_deg_30.mat --embedding embeddings/blogcatalog_subset_5000_sc_theta_90_deg_30.NetMF.embedding.npy --seed 0 --C 1 --start-train-ratio 10 --stop-train-ratio 90 --num-train-ratio 9
