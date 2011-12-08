/*
 *  Transcoder.h
 *  
 *
 *  Created by Sarantorn Bisalbutra on 11/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "unistd.h"


#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/common.h"
#include "libavutil/mathematics.h"

struct transcoder_data {
	AVFormatContext *inputFormatCtx;
	//Declaration for Audio stream//
	AVCodec *AudioCodec;
	AVCodecContext *AudioCodecCtx;

	AVCodec *AudioCodecEN;
	AVCodecContext *AudioCodecCtxEN;

	int infileno, outfileno;
};

int init_transcoder();
int init_transcoder_data(int infileno, int outfileno, struct transcoder_data *data);
int stream_differentiator(AVFormatContext* inputFormatCtx);
void audio_transcode(struct transcoder_data *data);
void free_transcode_data(struct transcoder_data *data);
