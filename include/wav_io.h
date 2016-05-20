// WAVE file header format
// Source: http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
#ifndef WAVPARSER_H
#define WAVPARSER_H

#include <stdio.h>

#define HEADER_SIZE 44

typedef struct {
    unsigned char native_header[HEADER_SIZE];   // Full native header
    unsigned char riff[4];                      // RIFF string
    unsigned int overall_size;                  // overall size of file in bytes
    unsigned char wave[4];                      // WAVE string
    unsigned char fmt_chunk_marker[4];          // fmt string with trailing null char
    unsigned int length_of_fmt;                 // length of the format data
    unsigned int format_type;                   // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
    unsigned int channels;                      // no.of channels
    unsigned int sample_rate;                   // sampling rate (blocks per second)
    unsigned int byterate;                      // SampleRate * NumChannels * BitsPerSample/8
    unsigned int block_align;                   // NumChannels * BitsPerSample/8
    unsigned int bits_per_sample;               // bits per sample, 8- 8bits, 16- 16 bits etc
    unsigned char data_chunk_header [4];        // DATA string or FLLR string
    unsigned int data_size;                     // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
    FILE * ptr;                                 // sound data FILE pointer
} HEADER;


long get_size_of_each_sample(HEADER *header);

long get_num_samples(HEADER *header);

int read_headers(HEADER *header, FILE *ptr);

int write_headers(HEADER *header, FILE *ptr);

int write_steg_sound_data(HEADER *header, FILE *ptr, char* msg, size_t msg_size, int mode);

int read_steg_sound_data(HEADER * header, char* msg, size_t msg_size, int mode);

#endif