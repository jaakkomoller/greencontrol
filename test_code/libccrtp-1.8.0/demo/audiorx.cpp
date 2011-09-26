// audiorx.
// A simple and amusing program for testing basic features of ccRTP.
// Copyright (C) 2001,2002  Federico Montesino <fedemp@altern.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// A very simple mu-law encoded audio player.

// This is an introductory example file that illustrates basic usage
// of ccRTP. You will also see a bit on how to use CommonC++ threads and
// TimerPort.

// I am a player of \mu-law encoded RTP audio packets. I
// do not accept any arguments.

#include <cstdio>
#include <cstdlib>
// Some consts common to audiorx and audiotx
#include "audio.h"
// In order to use ccRTP, the RTP stack of CommonC++, you only need to
// include ...
#include <ccrtp/rtp.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#define BUFSIZE 1024

#include <ao/ao.h>  
#include <math.h>  

#ifdef  CCXX_NAMESPACES
using namespace ost;
using namespace std;
#endif

/**
 * @class ccRTP_AudioReceiver
 * This is the class that will do almost everything.
 */
class ccRTP_AudioReceiver: public Thread, public TimerPort
{
private:
    // This is the file we will write to (/dev/audio)
    int audiooutput;
    // The aforementioned file will be transmitted through this socket
    RTPSession *socket;

public:
    // Constructor
    ccRTP_AudioReceiver(int fn) {
        //audiooutput=open("/dev/snd/default",O_WRONLY/*|O_NDELAY*/);
	//audiooutput=STDIN_FILENO;
	audiooutput=fn;
        /*if( audiooutput > 0 ) {
            cout << "Audio device is ready to play." << endl;
        }else{
            cout << "I could not open /dev/audio " << endl;
            exit();
        }*/

        socket=NULL;
    }

    // Destructor.
    ~ccRTP_AudioReceiver() {
        terminate();
        delete socket;
        ::close(audiooutput);
    }

    // This method does almost everything.
    void run(void) {
        // redefined from Thread.

        // Before using ccRTP you should learn something about other
        // CommonC++ classes. We need InetHostAddress...

        // Construct loopback address
        InetHostAddress local_ip;
        local_ip = "127.0.0.1";

        // Is that correct?
        if( ! local_ip ) {
        // this is equivalent to `! local_ip.isInetAddress()'
            cerr << ": IP address is not correct!" << endl;
            exit();
        }

        cout << local_ip.getHostname() <<
            " is going to listen to perself through " <<
            local_ip << "..." << endl;

        // ____Here comes the real RTP stuff____

        // Construct the RTP socket
        socket = new RTPSession(local_ip,RECEIVER_BASE,0);

        // Set up receiver's connection
        socket->setSchedulingTimeout(10000);
        if( !socket->addDestination(local_ip,TRANSMITTER_BASE) )
            cerr << "The receiver could not connect.";

        // Let's check the queue (you should read the documentation
        // so that you know what the queue is for).
        socket->startRunning();
        cout << "The RTP queue is ";
        if( socket->isActive() )
            cout << "active." << endl;
        else
            cerr << "not active." << endl;

        cout << "Waiting for audio packets..." << endl;

        // This will be useful for periodic execution.
        TimerPort::setTimer(PERIOD);

        setCancel(cancelImmediate);
        // This is the main loop, where packets are sent and receipt.
        socket->setPayloadFormat(StaticPayloadFormat(sptPCMU));
        for( int i=0 ; true ; i++ ) {
            const AppDataUnit* adu;
            do {
                adu = socket->getData(socket->getFirstTimestamp());
                if ( NULL == adu )
                    Thread::sleep(5);
                else cout << ".";
            }while ( (NULL == adu) || (adu->getSize() <= 0) );


            // This is for buffering some packets at the
            // receiver side, since playing smoothly
            // without any reception buffer is almost
            // impossible.  Try commenting the two lines
            // below, or stop transmission and continue
            // later: you will probably hear noise or
            // cracks.
            if (i==0)
                Thread::sleep(20);

            if(::write(audiooutput,adu->getData(),adu->getSize()) < (ssize_t)adu->getSize())
                break;

            cout << "." << flush;

            // Let's wait for the next cycle
            Thread::sleep(TimerPort::getTimer());
            TimerPort::incTimer(PERIOD);
        }

    } // end of run
};


int pulse(char *pn, int fn);

int main(int argc, char *argv[])
{
    cout << "This is audiorx, a simple test program for ccRTP." << endl;
    cout << "I am waiting for audio packets on port " << RECEIVER_BASE
         << "." << endl;
    cout << "Do you want to hear something? Run audiotx." << endl;
    cout << "Strike [Enter] when you are fed up. Enjoy!." << endl;

    int fd[2];
    if(pipe(fd) != 0) {
        cout << "error while allocating pipe";
        exit(1);
    }

    // Construct the main thread.
    ccRTP_AudioReceiver *receiver = new ccRTP_AudioReceiver(fd[1]);


    // Run it.
    receiver->start();

    pulse(argv[0], fd[0]);

    for(;1;)
        Thread::sleep(100);
    //cin.get();

    cout << endl << "That's all." << endl;

    delete receiver;

    exit(0);
}


int pulse(char *pn, int fn) {

    /* The Sample format to use */
    static pa_sample_spec ss;
    ss.format = PA_SAMPLE_ULAW;
    ss.rate = 8000;
    ss.channels = 1;
    

    pa_simple *s = NULL;
    int ret = 1;
    int error;

    /* replace STDIN with the specified file if needed */
    /*if (argc > 1) {
        int fd;

        if ((fd = open(argv[1], O_RDONLY)) < 0) {
            fprintf(stderr, __FILE__": open() failed: %s\n", strerror(errno));
            goto finish;
        }

        if (dup2(fd, STDIN_FILENO) < 0) {
            fprintf(stderr, __FILE__": dup2() failed: %s\n", strerror(errno));
            goto finish;
        }

        close(fd);
    }*/

    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, pn, PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    for (;;) {
        uint8_t buf[BUFSIZE];
        ssize_t r;

#if 0
        pa_usec_t latency;

        if ((latency = pa_simple_get_latency(s, &error)) == (pa_usec_t) -1) {
            fprintf(stderr, __FILE__": pa_simple_get_latency() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        fprintf(stderr, "%0.0f usec    \r", (float)latency);
#endif

        /* Read some data ... */
        if ((r = read(fn, buf, sizeof(buf))) <= 0) {
            if (r == 0) /* EOF */
                break;

            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            goto finish;
        }
//printf("read data");
        /* ... and play it */
        if (pa_simple_write(s, buf, (size_t) r, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
        }
    }

    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    ret = 0;

finish:

    if (s)
        pa_simple_free(s);

    return ret;
}


/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 4
 * End:
 */



