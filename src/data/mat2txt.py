#!/usr/bin/env python
# encoding: utf-8
# File Name: mat2edge.py
# Author: Jiezhong Qiu
# Create Time: 2019/03/18 12:01
# TODO:

import scipy.io
import scipy.sparse as sparse
import math
import numpy as np
import sys

def WriteMatrix(file, A):
    A = A.todok()
    print(A.shape)
    print(A.nnz)
    with open(file, "w") as f:
        f.write("%d %d %d\n" % (A.shape[0], A.shape[1], A.nnz));
        for (x, y), v in A.items():
            f.write("%d %d\n" % (x, y))

def mat2txt(name):
    print(name)
    print("turning %s into text" % (name))
    data = scipy.io.loadmat(name + ".mat")

    WriteMatrix(name + ".network.txt", sparse.triu(data["network"]));
    WriteMatrix(name + ".group.txt", data["group"]);

if __name__ == "__main__":
        #mat2txt("youtube_connected");
#       mat2txt("Data/blogcatalog");
#       mat2txt("Data/flickr");
        mat2txt(sys.args[1]);
