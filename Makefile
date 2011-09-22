CC=gcc
CFLAGS=
LDFLAGS=
SOURCES=main.c rtp_server.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=RadioStreamer

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
