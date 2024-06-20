/*
 * waveform.c
 *
 * This program generates PNG images representing waveform data from a FLAC file.
 *
 * Copyright (C) 2024 Kryštof Pešek / Kof
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <FLAC/all.h>
#include <png.h>

#define WIDTH 1920
#define HEIGHT 1080 

void save_png(const char *filename, unsigned char data[2][WIDTH]) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    png_set_IHDR(
        png,
        info,
        WIDTH, HEIGHT,
        8,
        PNG_COLOR_TYPE_GRAY,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    png_bytep row = (png_bytep) malloc(WIDTH * sizeof(png_byte));
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            row[x] = (y < HEIGHT / 2) ? data[0][x] : data[1][x];
        }
        png_write_row(png, row);
    }

    png_write_end(png, NULL);

    fclose(fp);
    png_free_data(png, info, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png, &info);
    free(row);
}

FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data) {
    static unsigned int samples = 0;
    static unsigned int image_count = 0;
    unsigned char (*data)[WIDTH] = client_data;

    FLAC__int32 min_val = 0;
    FLAC__int32 max_val = 0;
    double range;
    double factor;



    // Find the minimum and maximum values in the samples array
    for (int i = 0; i < WIDTH * 2; i++) {
        if (buffer[0][i] < min_val) {
            min_val = buffer[0][i];
        }

        if (buffer[0][i] > max_val) {
            max_val = buffer[0][i];
        }
    }

   range = (double)(max_val - min_val);
    factor = 255.0 / range;
 

    // Find the minimum and maximum values in the samples array
    for (size_t i = 0; i < frame->header.blocksize; i++) {
		for (int channel = 0; channel < 2; channel++) {

          

 		// Normalize and scale the sample to 0-255
        double scaled_value = ((double)(buffer[channel][i] - min_val)) * factor;
        
        // Ensure the value is within the valid range for uint8_t
        if (scaled_value < 0.0) {
            scaled_value = 0.0;
        } else if (scaled_value > 255.0) {
            scaled_value = 255.0;
        }

        // Convert to uint8_t
        //uint8_samples[i] = (uint8_t)scaled_value;
       	data[channel][samples] = (uint8_t)scaled_value;
        

}

samples++;
        if (samples == WIDTH) {
            char filename[50];
            sprintf(filename, "waveform_%06d.png", image_count++);
            save_png(filename, data);
            samples = 0;
        }
}         
       
    

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
    fprintf(stderr, "Error: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.flac>\n", argv[0]);
        return 1;
    }

    FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
    if (!decoder) {
        fprintf(stderr, "ERROR: allocating decoder\n");
        return 1;
    }

    unsigned char data[2][WIDTH] = {0};

    if (FLAC__stream_decoder_init_file(decoder, argv[1], write_callback, NULL, error_callback, data) != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        fprintf(stderr, "ERROR: initializing decoder\n");
        FLAC__stream_decoder_delete(decoder);
        return 1;
    }

    FLAC__stream_decoder_process_until_end_of_stream(decoder);
    FLAC__stream_decoder_finish(decoder);
    FLAC__stream_decoder_delete(decoder);

    return 0;
}
