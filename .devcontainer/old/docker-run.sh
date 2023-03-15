#!/usr/bin/env bash

image_name="arctic-engine"
container_name="arctic-engine"

# kill and stop the container
docker kill $container_name > /dev/null 2>&1 || true
docker rm $container_name > /dev/null 2>&1 || true

# run the container
docker run -d -it -p 2222:22 --name $container_name $image_name
docker container exec $container_name systemctl start ssh