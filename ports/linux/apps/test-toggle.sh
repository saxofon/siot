#!/bin/bash

while true; do
	LD_LIBRARY_PATH=../libs/siot ./load-control t
	sleep 1
done
