make clean
make client
touch scale.txt
./client 1 800 1 >> scale.txt
./client 2 400 1 >> scale.txt
./client 4 200 1 >> scale.txt