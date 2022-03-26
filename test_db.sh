#!/bin/bash
git fetch
git pull
make clean
make db_test
./db_test