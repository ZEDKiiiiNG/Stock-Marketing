make clean
make client
touch scale.txt
echo "1 core"
./client 1 800 1 >> scale.txt
echo "2 core "
./client 2 400 1 >> scale.txt
echo "4 core"
./client 4 200 1 >> scale.txt