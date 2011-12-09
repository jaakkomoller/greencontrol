#ifndef MP3FETCHER_H
#define MP3FETCHER_H

#define MAX_STATIONS 4

int fetch_file(char *, char *);
int fetch_playlist(char *);
int fetch_page(char *, char *, char *, char *);
char* parseString(char *);
int fetch_station_info(char[][100], int);
int start_gui(int, int *, char [][100], int);

#endif

