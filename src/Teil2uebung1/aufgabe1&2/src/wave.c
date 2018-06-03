#define _CRT_SECURE_NO_WARNINGS

#include "wave.h"
#include <stdio.h>
#include <string.h>

void writePCM(char * name, float *signal, int N, wavheader header)
{
  FILE *fileOut = fopen(name,"wb");
  chunkheader dataheader;

  header.riff_chunk_header.chunk_size = 
    /*Größe chunks RIFF+fmt */ sizeof(header)/*36*/ - sizeof(chunkheader) /*8*/ /*subchunk header RIFF zählt nicht mit*/ + 
    /*Größe subchunk data*/ sizeof(chunkheader) /*8*/ + N*4 ;
  fwrite(&header,sizeof(header),1,fileOut);

  strncpy(dataheader.chunk_id,"data",4);
  dataheader.chunk_size=N*4;
  fwrite(&dataheader,sizeof(dataheader),1,fileOut);
  fwrite(signal,4,N,fileOut);

  fclose(fileOut);
}
