#!/bin/bash
set -x #echo on
g++ -c -g -fPIC -Wall ptx_from_fatbin.cpp
g++ -c -g -fPIC -Wall intercept_fatbin.cpp
g++ -g -Wall -shared -o libextractptx_6_0.so ptx_from_fatbin.o intercept_fatbin.o
