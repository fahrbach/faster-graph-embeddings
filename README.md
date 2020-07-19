# Faster Graph Embeddings via Coarsening

This repository contains the source code for the experiments in
[Faster Graph Embeddings via Coarsening](https://arxiv.org/abs/2007.02817).

## Summary

We investigate how coarsening algorithms based on the Schur complement affect
the predictive performance of random walk-based graph embeddings for different
learning tasks.
Our multi-label vertex classification experiment builds on the framework for
NetMF and evaluates the accuracy of one-vs-all logistic regression models.
Our link prediction experiment builds on the setup in node2vec and explores the
effect of vertex sparsification on AUC scores for several popular baselines.

## Citation

If you find this code useful in your research, we ask that you cite the
following paper:

```
@article{fahrbach2020faster,
  title={Faster Graph Embeddings via Coarsening},
  author={Fahrbach, Matthew and Goranci, Gramoz and Peng, Richard and Sachdeva, Sushant and Wang, Chi},
  journal={arXiv preprint arXiv:2007.02817},
  year={2020}
}
```
