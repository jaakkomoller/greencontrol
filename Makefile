CC=gcc
CFLAGS=-g
LDFLAGS=-lavutil -lavformat -lavcodec -lgcc
SOURCES=main.c rtp_connection.c rtp_packet.c util.c mp3fetcher.c converter.c Transcoder.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=RadioStreamer

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)
