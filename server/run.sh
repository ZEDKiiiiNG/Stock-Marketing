#!/bin/bash
make clean
make server
./server
./code/server
echo "hello"
while true
do
    sleep 1
done
