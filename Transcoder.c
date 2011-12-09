/*
 *  Transcoder.c
 *  
 *
 *  Created by Sarantorn Bisalbutra on 11/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 *	Compilation
 *	gcc -std=c99 -g -pedantic -Wall -Wextra -lavutil -lavformat -lavcodec -lz -lm `sdl-config --cflags --libs` -o Transcoder Transcoder.c
 *
 */

#include "Transcoder.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h> /* dup */
#include <sys/file.h> /* O_CREAT jne*/
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>


#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/common.h"
#include "libavutil/mathematics.h"

#define AUDIO_INBUF_SIZE AVCODEC_MAX_AUDIO_FRAME_SIZE

int init_transcoder() {
	avcodec_init();
	av_register_all();
}

int init_transcoder_data(int transcoder_inno, int transcoder_outno, struct transcoder_data *data) {

	char infilename[100];

	data->inputFormatCtx = NULL;
	data->AudioCodec = NULL;
	data->AudioCodecCtx = NULL;

	data->AudioCodecEN = NULL;
	data->AudioCodecCtxEN = NULL;

	data->transcoder_in = transcoder_inno;
	data->transcoder_out = transcoder_outno;

	data->initialized = 0;

	sprintf(infilename, "pipe:%d", data->transcoder_in);

	// Open audio file
	if(av_open_input_file(&data->inputFormatCtx, infilename, NULL, 0, NULL)!=0){
		printf("couldn't open audio file\n");
		exit(1); // Couldn't open file
	}

	// Retrieve stream information
	if(av_find_stream_info(data->inputFormatCtx) < 0){
		printf("couldn't find stream\n");
		exit(1); // Couldn't find stream information
	}

	/* find the mpeg3 audio decoder */
	data->AudioCodecCtx = data->inputFormatCtx->streams[0]->codec;
	data->AudioCodec = avcodec_find_decoder(data->AudioCodecCtx->codec_id);
	if (!data->AudioCodec) {
		fprintf(stderr, "Decoding codec not found\n");
		exit(1);
	}

	data->AudioCodecCtx = avcodec_alloc_context();

	/* Initializes the AVCodecContext to use the given AVCodec */
	if (avcodec_open(data->AudioCodecCtx, data->AudioCodec) < 0) {
		fprintf(stderr, "could not open codec for decoder\n");
		exit(1);
	}

	data->AudioCodecEN = avcodec_find_encoder(CODEC_ID_PCM_MULAW);
	if (!data->AudioCodecEN) {
		fprintf(stderr, "Encoding codec not found\n");
		exit(1);
	}

	data->AudioCodecCtxEN = avcodec_alloc_context();
	data->AudioCodecCtxEN->sample_rate = 8000;
	data->AudioCodecCtxEN->channels = 1;
	data->AudioCodecCtxEN->bits_per_raw_sample = 8;
	data->AudioCodecCtxEN->sample_fmt = AV_SAMPLE_FMT_S16;

	if (avcodec_open(data->AudioCodecCtxEN, data->AudioCodecEN) < 0) {
		fprintf(stderr, "could not open codec for encoder\n");
		exit(1);
	}

}

static void encode(int in, int out, AVCodecContext *AudioCodecCtxEN, long int sample_rate) {

	int i = 0, bytes_available = 0, bytes_to_read = 0, out_sizeEN = 0, outbuf_size = 0, err = 0;
	uint8_t *buf_in;
	uint16_t *outbufEN;
	float ratio;
	uint8_t sample;

	outbuf_size = 2;
	outbufEN = (uint16_t *)malloc(sizeof(uint8_t) * outbuf_size);
	memset(outbufEN, 0, sizeof(uint8_t) * outbuf_size);
	ratio = sample_rate / 8000.0;  // sample per second
	bytes_to_read = ratio * 4; 
	buf_in = (uint8_t *)malloc(sizeof(uint8_t) * AVCODEC_MAX_AUDIO_FRAME_SIZE);
	i=0;

	// Check amount of data in pipe
	err = ioctl(in, FIONREAD, &bytes_available);
	if(err != 0) {
		perror("Error while checking size of pipe: ");
		exit(1);
	}

	while(bytes_available >= bytes_to_read && read(in, buf_in, bytes_to_read) > 0 && i < 60000){
		out_sizeEN = avcodec_encode_audio(AudioCodecCtxEN, (uint8_t *)outbufEN, outbuf_size, (short int*)(buf_in));
		sample = *outbufEN / 256;
		write(out, &sample, 1);

		// Check amount of data in pipe
		err = ioctl(in, FIONREAD, &bytes_available);
		if(err != 0) {
			perror("Error while checking size of pipe: ");
			exit(1);
		}
		i++;
	}

	free(outbufEN);
}

void audio_transcode(struct transcoder_data *data, int *state)
{
	int out_size, length, j, a, bytes_available, err;
	uint8_t *outbuf;
	uint8_t *inbuf;

	int p[2]; // This pipes data from decode to encode

	AVPacket Audiopkt;

	av_init_packet(&Audiopkt);

	if(pipe(p) != 0) {
		perror("Error while creating pipe: ");
		exit(1);
	} 
	fcntl(p[0], F_SETFL, O_NONBLOCK);

	out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	outbuf = malloc(out_size); // AVCODEC_MAX_AUDIO_FRAME_SIZE = 192000 bytes

	int64_t inbuf_size = AUDIO_INBUF_SIZE;
	inbuf = malloc(inbuf_size + FF_INPUT_BUFFER_PADDING_SIZE); //AUDIO_INBUF_SIZE  = 20480 bytes

	/* decode until eof */
	Audiopkt.data = inbuf;

	while((av_read_frame(data->inputFormatCtx, &Audiopkt)) >= 0){
		while(Audiopkt.size > 0) {
			out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
			length = avcodec_decode_audio3(data->AudioCodecCtx, (short*) outbuf, &out_size, &Audiopkt);// outbuf = decompressed frame size in bytes

			if(length < 0) {
				break;
			}
			if (out_size > 0) {
				/*if a frame has been decoded, output it*/ 
				write(p[1], outbuf, out_size);
				encode(p[0], data->transcoder_out, data->AudioCodecCtxEN, data->inputFormatCtx->streams[0]->codec->sample_rate);
			}
			Audiopkt.size -= length;
			Audiopkt.data += length;
			memset(outbuf, 0, sizeof(uint8_t) * AVCODEC_MAX_AUDIO_FRAME_SIZE);
		}
	}

	printf("transcoder quitting\n");

	free(outbuf);
	free(inbuf);
}

void free_transcode_data(struct transcoder_data *data) {
	avcodec_close(data->AudioCodecCtx);
	av_free(data->AudioCodecCtx);
	avcodec_close(data->AudioCodecCtxEN);
	av_free(data->AudioCodecCtxEN);
}



