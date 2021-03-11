#!/bin/bash

docker run -it --privileged --rm -v "$PWD":/project -w /project espressif/idf idf.py flash