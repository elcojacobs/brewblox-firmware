#!/bin/bash

docker run --rm -v "$PWD":/project -w /project espressif/idf idf.py build