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

int stream_differentiator(AVFormatContext* inputFormatCtx);
static void audio_transcode(const char *inputfile, const char *outputfile);
