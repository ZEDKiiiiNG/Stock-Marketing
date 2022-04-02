git reset --hard HEAD
git fetch
git pull
cd server
chmod +x run.sh
cd ..
sudo docker-compose up --build