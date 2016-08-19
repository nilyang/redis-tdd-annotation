#!/bin/bash


gcc -D SDS_TEST_MAIN  -Wall  -ggdb  hello_sds.c -o sds
valgrind --vgdb=yes --leak-check=full ./sds

