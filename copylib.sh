#!/bin/bash
your_program=$1
output=$2

# Replace "your_program" with the path to your executable
for i in $(ldd $your_program | awk '{ print $3 }' | grep "^/"); do
    cp -- "$i" ./$output
done