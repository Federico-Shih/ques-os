#docker run -d -v "${PWD}:/root" --security-opt seccomp:unconfined -ti --name arqui agodio/itba-so:1.0
docker start $1
docker exec -it $1 make clean    -C /root/Toolchain
docker exec -it $1 make all      -C /root/Toolchain
docker exec -it $1 make clean    -C /root/
docker exec -it $1 make all      -C /root/
docker stop $1
#docker rm arqui
