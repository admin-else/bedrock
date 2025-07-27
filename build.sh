#!/bin/sh
rm bedrock
clang main.c -O3 -lssl -lcrypto -o bedrock
