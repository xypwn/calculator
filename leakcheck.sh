#!/usr/bin/env sh
make
EXPR="2.5/(5+3)-2*4/3.3^4.1^5" 
echo "$EXPR" | valgrind --leak-check=yes ./str_eval
