#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Usage: $0 input.xxx output.wav"
	exit 1
fi

ffmpeg -y -loglevel quiet -i $1 -c:a pcm_s16le -vn $2
