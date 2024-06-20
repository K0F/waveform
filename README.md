# Waveform

_A simple C program which renders FLAC files to something as waveform. Grayscale stripes are colored by every value in the stereo stream. Theoretically it is phonograph, you should be able to reconstruct 8-bit crappy sound back from it :)_

## Prerequesties

  - libPNG
  - libFLAC

On debian:
    
`sudo apt install libpng-dev libflac-dev`

On arch:

 `sudo pacman -S libpng libFLAC`

## Compile

Standard Makefile:

`make`

## Demo

[Demo](https://www.youtube.com/watch?v=Du2W8XubCf8)

## Usage

It currently support only s32 FLAC stereo files, well advised is 48000 audiorate... it renders a series of grayscale numbered PNGs in FullHD resolution. No flags nor switches.

`./waveform test.flac`

Resulting 25 1920x1080 images per second of recording in current folder.

## Stitching the result

FFmpeg will do, i.e. like this:

`ffmpeg -r 25 -i waveform_%06d.png -i input.flac -c:v h264 -qp 19 -c:a mp3 -b:a 256k -pix_fmt yuv420p -r 25 -y output.mp4`

