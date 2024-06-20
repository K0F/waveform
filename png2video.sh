#!/bin/sh
ffmpeg -r 25 -i waveform_%06d.png -i "$1" -c:v h264 -qp 19 -c:a mp3 -b:a 256k -pix_fmt yuv420p -r 25 -y output.mp4
