#!/usr/bin/env bash

image_name="arctic-engine"
container_name="arctic-engine"

docker run --rm -d -it --name $container_name $image_name