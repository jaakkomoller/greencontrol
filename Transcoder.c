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
//#include <wait.h>


#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/common.h"
#include "libavutil/mathematics.h"


#define AUDIO_REFILL_THRESH 4096

/*
int main(int argc, char *argv[]) 
{
	
	char* infilename,*pcmfilename;
	
	if(argc < 2) {
		fprintf(stderr, "Usage: stage1prog <file>\n");
		exit(1);
	}
	
	infilename = argv[1];
	pcmfilename = argv[2];
	
	// must be called before using avcodec lib 
	avcodec_init();
	// register all the codecs 
	av_register_all();
	
	int fd;
	pid_t pid;
	int status;
	
	fd=open("warning.txt", O_CREAT|O_WRONLY|O_APPEND,S_IRUSR|S_IWUSR);
	pid = fork(); 	
	if (pid==0)
	{
		dup2(fd, 2); 
		
		audio_transcode(infilename,pcmfilename);
		close(fd);
	}
	else{
		if(waitpid(pid,&status,WUNTRACED) == -1 ){
			return 1;
		}
	}
	
	return(0);
	
}*/

int init_transcoder() {
	avcodec_init();
	av_register_all();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int stream_differentiator(AVFormatContext* inputFormatCtx)
{
	
	unsigned int	i; 
	int videoStream, audioStream;
	
	// Find the first video stream
	videoStream=-1;
	audioStream=-1;
	
	for(i=0; i<inputFormatCtx->nb_streams; i++) {
		if(inputFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO &&
		   videoStream < 0) {
			videoStream=i;
			printf("VDO at i = %d\n",i);
		}
		if(inputFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO &&
		   audioStream < 0) {
			audioStream=i;
			printf("Audio at i = %d\n",i);
		}
	}
	if(videoStream==-1)
		printf("There is no VDO stream in this file\n"); // Didn't find a video stream
	if(audioStream==-1)
		printf("There is no Audio stream in this file\n");
	return(audioStream);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void audio_transcode(int infile, int outfile)
{
	
	AVFormatContext *inputFormatCtx;
	//Declaration for Audio stream//
	AVCodec *AudioCodec = NULL;
	AVCodecContext *AudioCodecCtx= NULL;
	AVPacket Audiopkt;
	int out_size, length, AudioPosition;
//	FILE *infile, *outfile;
	FILE * tempfile = tmpfile();
	uint8_t *outbuf;
	uint8_t *inbuf;
	long int sample_rate;
	char infilename[100];

	strcp(infilename, "pipe:");
	strcp(&infilename[5], itoa(infile));
	
	AVCodec *AudioCodecEN = NULL;
    AVCodecContext *AudioCodecCtxEN= NULL;
    uint8_t *outbufEN, *buffer_file;
    int bytes_to_read, out_sizeEN, outbuf_size;
    float ratio, j;
	

	// Open audio file
	if(av_open_input_file(&inputFormatCtx, infilename, NULL, 0, NULL)!=0){
		printf("couldn't open audio file\n");
		exit(1); // Couldn't open file
	}
	
	
	// Retrieve stream information
	if(av_find_stream_info(inputFormatCtx) < 0){
		printf("couldn't find stream\n");
		exit(1); // Couldn't find stream information
	}
	
	if(inputFormatCtx->nb_streams > 1)
		AudioPosition = stream_differentiator(inputFormatCtx);
	else 
		AudioPosition = 0;
	
	
	//unlink(outputfile);

	/* find the mpeg3 audio decoder */
	AudioCodecCtx = inputFormatCtx->streams[AudioPosition]->codec;
	AudioCodec = avcodec_find_decoder(AudioCodecCtx->codec_id);
	if (!AudioCodec) {
		fprintf(stderr, "Decoding codec not found\n");
		exit(1);
	}
	
	AudioCodecCtx= avcodec_alloc_context();
	
	/* Initializes the AVCodecContext to use the given AVCodec */
	if (avcodec_open(AudioCodecCtx, AudioCodec) < 0) {
		fprintf(stderr, "could not open codec for decoder\n");
		exit(1);
	}
	
	outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE); // AVCODEC_MAX_AUDIO_FRAME_SIZE = 192000 bytes
	
	int64_t AUDIO_INBUF_SIZE = inputFormatCtx->file_size;
	inbuf = malloc(AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE); //AUDIO_INBUF_SIZE  = 20480 bytes
	//FF_INPUT_BUFFER_PADDING_SIZE  = 8 bytes
	
	
	/*find audio encoder*/
	AudioCodecEN = avcodec_find_encoder(CODEC_ID_PCM_MULAW);
    if (!AudioCodecEN) {
        fprintf(stderr, "Encoding codec not found\n");
        exit(1);
    }
    
    AudioCodecCtxEN = avcodec_alloc_context();
    AudioCodecCtxEN->sample_rate = 8000;
    AudioCodecCtxEN->channels = 1;
	AudioCodecCtxEN->bits_per_raw_sample = 8;
	
    if (avcodec_open(AudioCodecCtxEN, AudioCodecEN) < 0) {
        fprintf(stderr, "could not open codec for encoder\n");
        exit(1);
    }
	
	outbuf_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	outbufEN = (uint8_t*)malloc(sizeof(uint8_t)*outbuf_size);
	memset(outbufEN, 0, sizeof(uint8_t)*outbuf_size);
	buffer_file = (uint8_t*)malloc(sizeof(uint8_t)*outbuf_size);
	sample_rate = inputFormatCtx->streams[AudioPosition]->codec->sample_rate;
	ratio = sample_rate / 8000.0;  // sample per second
	bytes_to_read = ratio * 4; 
	
	/* file */
	
/*	infile = fopen(inputfile, "rb");
	if (!infile) {
		fprintf(stderr, "could not open %s\n", inputfile);
		exit(1);
	}
	
	outfile = fopen(outputfile, "wb");
	if (!outfile) {
		av_free(AudioCodecCtx);
		exit(1);
	}
*/	
	/* decode until eof */
	Audiopkt.data = inbuf;

	int i = 0;
	while((av_read_frame(inputFormatCtx, &Audiopkt)) >= 0){
		out_size=AVCODEC_MAX_AUDIO_FRAME_SIZE;
		if(Audiopkt.stream_index==AudioPosition){
			while(Audiopkt.size > 0) {
				length = avcodec_decode_audio3(AudioCodecCtx, (short*) outbuf, &out_size, &Audiopkt);// outbuf = decompressed frame size in bytes
				//printf("length %d  out_size = %d\n",length, (sizeof(outbuf)));
				//printf("%d",outbuf);
				if(length < 0) {
					break;
				}
				if (out_size > 0) {
					/*if a frame has been decoded, output it*/ 
					fwrite(outbuf, 1, out_size, tempfile);
				}
				Audiopkt.size -= length;
				Audiopkt.data += length;
				if (Audiopkt.size < AUDIO_REFILL_THRESH) {
					/* Refill the input buffer, to avoid trying to decode
					 * incomplete frames. Instead of this, one could also use
					 * a parser, or use a proper container format through
					 * libavformat.
					 * AUDIO_REFILL_THRESH = 4096*/
					memmove(inbuf, Audiopkt.data, Audiopkt.size);
					Audiopkt.data = inbuf;
					length = read(infile, Audiopkt.data + Audiopkt.size, AUDIO_INBUF_SIZE - Audiopkt.size);
					if (length > 0)
						Audiopkt.size += length;
				}
				memset(outbuf, 0, sizeof(uint8_t)*AVCODEC_MAX_AUDIO_FRAME_SIZE);
			}
		}
		i++;
	}

	outbuf_size = 1;
	j=0;
	i = 0;
	rewind (tempfile);
	while(fread(buffer_file, 1, bytes_to_read, tempfile)>0){
		out_sizeEN = avcodec_encode_audio(AudioCodecCtxEN, outbufEN, outbuf_size, (short int*)buffer_file);
		write(outfile, outbufEN, 1);
		i++;

	}
	
//	fclose(outfile);
//	fclose(infile);
	free(outbuf);
	free(outbufEN);
	free(inbuf);
	
	avcodec_close(AudioCodecCtx);
	av_free(AudioCodecCtx);
	avcodec_close(AudioCodecCtxEN);
    av_free(AudioCodecCtxEN);
	
}

