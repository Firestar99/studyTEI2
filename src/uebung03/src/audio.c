#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wave.h"

#define TRUE 1
#define FALSE 0

const int RIFF_HEADER_OFFSET = 0;
const int RIFF_HEADER_LENGTH = 4;
const int FMT_HEADER_OFFSET = 12;
const int FMT_HEADER_LENGTH = 4;

const int FORMAT_OFFSET = 8;
const int FORMAT_LENGTH = 4;
const int CHANNEL_OFFSET = 22;
const int SAMPLE_RATE_OFFSET = 24;
const int BITS_PER_SAMPLE_OFFSET = 34;
const int DISTANCE_ID_TO_SIZE = 4;
const int DISTANCE_ID_TO_DATA = 8;
const int FIELD_LENGTH = 4;

//utility
union charArrayToInt {
	char c[4];
	unsigned int i;
};

union charToFloat {
	char c[4];
	float f;
}

setNullTerminator(char* string, int length)
{
	string[length-1] = '\0';
}

unsigned int byteswap(void* num) {
	union charArrayToInt size;
   	memcpy(&size.c, num, 4);
   	return size.i;
}

//float data
unsigned int getPositionOfDataID(char* fileData, int fileLength)
{
	int pos = FMT_HEADER_OFFSET;
	while(pos < fileLength && memcmp(fileData+pos, &"data", 4) != 0)
	{
	    pos += DISTANCE_ID_TO_DATA + (*(fileData+pos+DISTANCE_ID_TO_SIZE));
		printf("SubchunkID: %.4s\n", fileData+pos);
	}
	if(!(memcmp(fileData+pos, &"data", 4) == 0))
	    exit;
	return pos;
}

unsigned int readDataChunk(char* fileData, int fileLength, float** data)
{
	unsigned int pos = getPositionOfDataID(fileData, fileLength);
    unsigned int length = byteswap(fileData+pos+DISTANCE_ID_TO_SIZE);
    printf("pos: %u\n", pos);
    printf("length: %u\n", length);
    *data = (float*) fileData+pos+DISTANCE_ID_TO_DATA;
    return length;
}

//writeback
void schneller(char* path, float *data, int length, wavheader wave)
{
	int halfLength = length / 2;
	float halfOfData[halfLength];
	for(int i = 0; i < halfLength; i++)
		halfOfData[i] = data[i*2];
	writePCM(path, halfOfData, halfLength, wave);
}

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

int main()
{
	char* fileData;
	int fileLength = readFile("test.wav", &fileData);
	wavheader wave = *((wavheader*)fileData);

	//print fields of the chunkheaders
	printf("RIFF ID: %.4s\n", wave.riff_chunk_header.chunk_id);
	printf("RIFF size: %i\n", wave.riff_chunk_header.chunk_size);
	printf("fmt ID: %.4s\n", wave.fmt_chunk_header.chunk_id);
	printf("fmt size: %i\n", wave.fmt_chunk_header.chunk_size);

	printf("riff Format: %.4s\n", wave.riff_format);
	printf("Channels: %i\n", wave.channels);
	printf("Sample Rate: %u\n", wave.sample_rate);
	printf("bits per sample: %u\n", wave.bits_per_sample);
	printf("length in seconds: %f\n", (float) wave.riff_chunk_header.chunk_size / (float) (wave.byterate));

	float* data;
	unsigned int length = readDataChunk(fileData, fileLength, &data);

    printf("readDataSuccess\n");
    writePCM("testClone.wav", data, length/4, wave);
	printf("writePCM done\n");
    schneller("testFaster.wav", data, length/4, wave);
	printf("schneller done\n");

	free(fileData);
	printf("done\n");
}
