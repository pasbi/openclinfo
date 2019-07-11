#!/usr/bin/env bash

cd $(dirname $0)
g++ -lOpenCL openclinfo.cpp && ./a.out
