#!/bin/bash

if [ $# -ne 3 ]; then
    echo "Usage: $0 field_id threshold filename"
    exit -1
else
    field=$1
    threshold=$2
    filename=$3
fi


awk -F',' "\$$field > $threshold" "$filename"
