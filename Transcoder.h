/*
 *  Transcoder.h
 *  
 *
 *  Created by Sarantorn Bisalbutra on 11/6/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef TRANSCODER_H
#define TRANSCODER_H

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
	AVCodec *AudioCodec;
	AVCodecContext *AudioCodecCtx;

	AVCodec *AudioCodecEN;
	AVCodecContext *AudioCodecCtxEN;

	int transcoder_in, transcoder_out, mp3_control, rtp_control, initialized;
};

int init_transcoder();
int init_transcoder_data(int transcoder_inno, int transcoder_outno, int mp3_control,
	int rtp_control, struct transcoder_data *data);
void audio_transcode(struct transcoder_data *data, int *state);
void free_context(struct transcoder_data *data);

#endif
