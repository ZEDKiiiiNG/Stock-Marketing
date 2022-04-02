#!/bin/bash
git fetch
git pull
make clean
make client
./client