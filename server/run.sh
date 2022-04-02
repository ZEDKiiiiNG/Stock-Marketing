#!/bin/bash
make clean
make server
echo "hello"
while [ "1"=="1" ]
do
    ./server
    sleep 1
done
