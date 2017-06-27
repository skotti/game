#!/bin/bash

if [ "$#" -ne 2 ]; then
	echo "Usage: $0 input.xxx output.ogg"
	exit 1
fi

ffmpeg -y -loglevel quiet -i $1 -c:a libvorbis -vn -q:a 6 $2
