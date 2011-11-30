#ifndef MP3FETCHER_H
#define MP3FETCHER_H

int fetch_file(char*,char*);
int fetch_playlist(char*);
int fetch_page(char*, char*,char*,char*);
char* parseString(char*);
int fetch_station_info();

#endif

