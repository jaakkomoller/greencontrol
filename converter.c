#include <stdio.h>
#include <stdlib.h>



int convert(int in_file, int out_file) {
	FILE *temp_in, *temp_out;
	int read_bytes = 0;
	char buf[1000];

	temp_in = popen("ffmpeg ffmpeg -i - -acodec pcm_mulaw -ac 1 -ar 8000 -f wav temp_out", "w");
	
	read_bytes = read(in_file, buf, 1000);
	while(read_bytes > 0) {
		fwrite (buf, 1, read_bytes, temp_in);
		read_bytes = read(in_file, buf, 1000);
	}

	//ffmpeg -i pipe:0 -acodec pcm_mulaw -ac 1 -ar 8000 -f wav -
	temp_out = fopen("temp_out", "r");

	read_bytes = fread (buf, 1, 1000, temp_out);	
	while(read_bytes > 0) {
		write(out_file, buf, read_bytes);
		read_bytes = fread (buf, 1, 1000, temp_out);	
	}


	return 0;
}
