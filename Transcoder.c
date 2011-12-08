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

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
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

/*int init_transcoder() {
	avcodec_init();
	av_register_all();
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int stream_differentiator(AVFormatContext* inputFormatCtx)
{
	
	unsigned int	i; 
	int videoStream, audioStream;
	
	// Find the first video stream
	videoStream=-1;
	audioStream=-1;
	
	for(i=0; i<inputFormatCtx->nb_streams; i++) {
		/*if(inputFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO &&
		   videoStream < 0) {
			videoStream=i;
			printf("VDO at i = %d\n",i);
		}
		if(inputFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO &&
		   audioStream < 0) {*/
		if(audioStream < 0) {
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

/*void audio_transcode(int infile, int outfile, char *filename)
{
    AVCodec *codec;
    AVCodecContext *c= NULL;
    int out_size, len;
    FILE *f;
    uint8_t *outbuf;
    uint8_t inbuf[AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
    AVPacket avpkt;

	avcodec_init();
	av_register_all();

    av_init_packet(&avpkt);

    printf("Audio decoding\n");

  */  /* find the mpeg audio decoder */
/*    codec = avcodec_find_decoder(CODEC_ID_MP3);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }

    c= avcodec_alloc_context();

  */  /* open it */
    /*if (avcodec_open(c, codec) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }

    outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

    f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "could not open %s\n", filename);
        exit(1);
    }*/
/*    outfile = fopen(outfilename, "wb");
    if (!outfile) {
        av_free(c);
        exit(1);
    }
*/
    /* decode until eof */
/*    avpkt.data = inbuf;
    printf("reading\n");
//    avpkt.size = read(infile, inbuf, AUDIO_INBUF_SIZE);
    avpkt.size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);
    printf("read\n");

    while (avpkt.size > 0) {
        out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
        len = avcodec_decode_audio3(c, (short *)outbuf, &out_size, &avpkt);
        if (len < 0) {
            fprintf(stderr, "Error while decoding\n");
    printf("reading\n");
	    avpkt.size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);
    printf("read\n");
//            exit(1);
            continue;
        }
        if (out_size > 0) {
*/            /* if a frame has been decoded, output it */
/*            printf("writing\n");
            write(outfile, outbuf, out_size);
            printf("wrote\n");
        }
        avpkt.size -= len;
        avpkt.data += len;
        if (avpkt.size < AUDIO_REFILL_THRESH) {
*/            /* Refill the input buffer, to avoid trying to decode
             * incomplete frames. Instead of this, one could also use
             * a parser, or use a proper container format through
             * libavformat. */
/*            memmove(inbuf, avpkt.data, avpkt.size);
            avpkt.data = inbuf;
            printf("reading\n");
//            len = read(infile, avpkt.data + avpkt.size,
//                        AUDIO_INBUF_SIZE - avpkt.size);
            len = fread(avpkt.data + avpkt.size, 1,
                    AUDIO_INBUF_SIZE - avpkt.size, f);
            printf("read\n");
            if (len > 0)
                avpkt.size += len;
        }
    }

//    fclose(outfile);
//    fclose(f);
    free(outbuf);

    avcodec_close(c);
    av_free(c);
}*/



int init_transcoder() {
	avcodec_init();
	av_register_all();
}

int init_transcoder_data(int infileno, int outfileno, struct transcoder_data *data) {
	
	char infilename[100];
	int AudioPosition = 0;

	data->inputFormatCtx = NULL;
	//Declaration for Audio stream//
	data->AudioCodec = NULL;
	data->AudioCodecCtx = NULL;

	data->AudioCodecEN = NULL;
	data->AudioCodecCtxEN = NULL;



	data->infileno = infileno;
	data->outfileno = outfileno;

	sprintf(infilename, "pipe:%d", infileno);

	// Open audio file
	if(av_open_input_file(&data->inputFormatCtx, infilename, NULL, 0, NULL)!=0){
		printf("couldn't open audio file\n");
		exit(1); // Couldn't open file
	}

	if(data->inputFormatCtx->nb_streams > 1)
		AudioPosition = stream_differentiator(data->inputFormatCtx);
	else 
		AudioPosition = 0;
	
	
	// Retrieve stream information
	if(av_find_stream_info(data->inputFormatCtx) < 0){
		printf("couldn't find stream\n");
		exit(1); // Couldn't find stream information
	}

	/* find the mpeg3 audio decoder */
	data->AudioCodecCtx = data->inputFormatCtx->streams[AudioPosition]->codec;
	data->AudioCodec = avcodec_find_decoder(data->AudioCodecCtx->codec_id);
	if (!data->AudioCodec) {
		fprintf(stderr, "Decoding codec not found\n");
		exit(1);
	}
	
	data->AudioCodecCtx= avcodec_alloc_context();
	
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
//    AudioCodecCtxEN->sample_fmt = AV_SAMPLE_FMT_U8;
	data->AudioCodecCtxEN->sample_fmt = AV_SAMPLE_FMT_S16;
	
	if (avcodec_open(data->AudioCodecCtxEN, data->AudioCodecEN) < 0) {
		fprintf(stderr, "could not open codec for encoder\n");
		exit(1);
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void audio_transcode(struct transcoder_data *data)
{
	
	int out_size, length, AudioPosition;
	FILE * tempfile = tmpfile();
	uint8_t *outbuf;
	uint8_t *inbuf;
	long int sample_rate;
	uint8_t sample;
	
	uint8_t *buffer_file;
	uint16_t *outbufEN;
	int bytes_to_read, out_sizeEN, outbuf_size;
	float ratio, j;
	
	AVPacket Audiopkt;
	
	if(data->inputFormatCtx->nb_streams > 1)
		AudioPosition = stream_differentiator(data->inputFormatCtx);
	else 
		AudioPosition = 0;
	
	
	//unlink(outputfile);

	outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE); // AVCODEC_MAX_AUDIO_FRAME_SIZE = 192000 bytes
	
//	int64_t AUDIO_INBUF_SIZE = inputFormatCtx->file_size;
	inbuf = malloc(AUDIO_INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE); //AUDIO_INBUF_SIZE  = 20480 bytes
	//FF_INPUT_BUFFER_PADDING_SIZE  = 8 bytes
	
	/*find audio encoder*/
//    AudioCodecEN = avcodec_find_encoder(CODEC_ID_PCM_U8);
	outbuf_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	outbufEN = (uint16_t*)malloc(sizeof(uint8_t)*outbuf_size);
	memset(outbufEN, 0, sizeof(uint8_t)*outbuf_size);
	buffer_file = (uint8_t*)malloc(sizeof(uint8_t)*outbuf_size);
	sample_rate = data->inputFormatCtx->streams[AudioPosition]->codec->sample_rate;
	ratio = sample_rate / 8000.0;  // sample per second
	bytes_to_read = ratio * 4; 
	
	/* file */
/*	
	infile = fopen(infilename, "rb");
	if (!infile) {
		fprintf(stderr, "could not open %s\n", infilename);
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
	while((av_read_frame(data->inputFormatCtx, &Audiopkt)) >= 0){
		out_size=AVCODEC_MAX_AUDIO_FRAME_SIZE;
		if(Audiopkt.stream_index==AudioPosition){
			while(Audiopkt.size > 0) {
				length = avcodec_decode_audio3(data->AudioCodecCtx, (short*) outbuf, &out_size, &Audiopkt);// outbuf = decompressed frame size in bytes
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
					length = read(data->infileno, Audiopkt.data + Audiopkt.size, AUDIO_INBUF_SIZE - Audiopkt.size);
//					length = fread(Audiopkt.data + Audiopkt.size, 1, AUDIO_INBUF_SIZE - Audiopkt.size, infile);
					if (length > 0)
						Audiopkt.size += length;
				}
				memset(outbuf, 0, sizeof(uint8_t)*AVCODEC_MAX_AUDIO_FRAME_SIZE);
			}
		}
		i++;
	}

	outbuf_size = 2;
	j=0;
	i = 0;
	rewind (tempfile);
	while(fread(buffer_file, 1, bytes_to_read, tempfile)>0, i < 60000){
		out_sizeEN = avcodec_encode_audio(data->AudioCodecCtxEN, (uint8_t *)outbufEN, outbuf_size, (short int*)buffer_file);
		printf("Writing\n");
		sample = *outbufEN / 256;
		write(data->outfileno, &sample, 1);
		printf("Wrote\n");
		i++;

	}
	
	free(outbuf);
	free(outbufEN);
	free(inbuf);
}

void free_transcode_data(struct transcoder_data *data) {
	avcodec_close(data->AudioCodecCtx);
	av_free(data->AudioCodecCtx);
	avcodec_close(data->AudioCodecCtxEN);
	av_free(data->AudioCodecCtxEN);
}



