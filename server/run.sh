#!/bin/bash
make clean
make server
./server
while true
do
    sleep 1
done
