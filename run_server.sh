#!/bin/bash
git fetch
git pull
make clean
make server
./server