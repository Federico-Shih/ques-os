docker start $1
docker exec -it $1 bash -c "cd ~ && gdb"
