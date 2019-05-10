#!/bin/sh
TARGET_DIR="./t/"
for file in `\find $TARGET_DIR -mindepth 1 -maxdepth 1 -type f | sort`; do
    /bin/echo "*******************************"
    ./test-cmp.sh "${file##*/}"
done
