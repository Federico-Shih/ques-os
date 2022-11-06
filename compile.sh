#!/usr/bin/env bash

if [ $1 == "docker"];
  then
    docker run -d -v "${PWD}:/root" --network="host" --security-opt seccomp:unconfined -ti --name $2 agodio/itba-so:1.0
elif [ $1 == "buddy" ];
  then
    docker start $2
    docker exec -it $2 make clean    -C /root/Toolchain
    docker exec -it $2 make all      -C /root/Toolchain
    docker exec -it $2 make clean    -C /root/
    docker exec -it $2 make all MM=BUDDY       -C /root/
    docker stop $2
elif [ $1 == "standard" ];
  then
    docker start $2
    docker exec -it $2 make clean    -C /root/Toolchain
    docker exec -it $2 make all      -C /root/Toolchain
    docker exec -it $2 make clean    -C /root/
    docker exec -it $2 make all MM=BUDDY       -C /root/
    docker stop $2
elif [ $1 == "clean" ];
  then
    docker start $2
    docker exec -it $2 make clean    -C /root/Toolchain
    docker exec -it $2 make clean    -C /root/
    docker stop $2
fi
