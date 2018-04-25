#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wave.h"

const int RIFF_HEADER_OFFSET = 0;
const int RIFF_HEADER_LENGTH = 4;
const int FMT_HEADER_OFFSET = 12;
const int FMT_HEADER_LENGTH = 4;

const int SUBCHUNK_SIZE_OFFSET = 4;
const int SUBCHUNK_DATA_OFFSET = 8;

//read file
int readFile(char* name, char** data) {
	FILE* wave;
	if((wave = fopen(name, "rb")) == NULL)
	{
		printf("Could not read File!");
		exit;
	}

	int seekRet = fseek(wave, 0, SEEK_END);
	if(seekRet != 0)
	{
		printf("fseek returned != 0");
		exit;
	}

	int length = ftell(wave);
	if(length == -1)
	{
		printf("ftell returned -1");
		exit;
	}
	rewind(wave);

	char* buffer = malloc(sizeof(char) * length);
	if(fread(buffer, sizeof(char), length, wave) != length)
	{
		printf("didn't read all the data!");
		exit;
	}

	fclose(wave);

	*data = buffer;
	return length;
}

//data chunk
unsigned int getPositionOfDataID(char* fileData, int fileLength)
{
	int pos = FMT_HEADER_OFFSET;
	while(pos < fileLength && memcmp(fileData+pos, &"data", 4) != 0)
	    pos += SUBCHUNK_DATA_OFFSET + (*(fileData+pos+SUBCHUNK_SIZE_OFFSET));
	if(!(memcmp(fileData+pos, &"data", 4) == 0))
	    exit;
	return pos;
}

unsigned int readDataChunk(char* fileData, int fileLength, float** data)
{
	unsigned int pos = getPositionOfDataID(fileData, fileLength);
    *data = (float*) (fileData+pos+SUBCHUNK_DATA_OFFSET);
    return *((unsigned int*) (fileData+pos+SUBCHUNK_SIZE_OFFSET));
}

//main
int main()
{
	char* fileData;
	int fileLength = readFile("test.wav", &fileData);
	wavheader wave = *((wavheader*) fileData);

	float* data;
	int length = readDataChunk(fileData, fileLength, &data) / 4;

    float average = 0;
    for(int i = 0; i < length; i++)
        average += fabs(data[i]);
    printf("average: %f\n", average/length);

	free(fileData);
	printf("done\n");
}
