#!/bin/bash
make clean
echo "hello1"
make server
./server
echo "hello2"
while true
do
    sleep 1
done
