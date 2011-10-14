CC=gcc
CFLAGS=
LDFLAGS=
SOURCES=main.c rtp_connection.c rtp_packet.c util.c mp3fetcher.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=RadioStreamer

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXECUTABLE)
