#!/bin/bash
DIR=$(dirname "$(readlink -f "$0")")
export LD_LIBRARY_PATH="$DIR/../lib":$LD_LIBRARY_PATH
$DIR/rdm
