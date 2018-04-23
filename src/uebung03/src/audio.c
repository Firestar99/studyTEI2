#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	{
	    pos += SUBCHUNK_DATA_OFFSET + (*(fileData+pos+SUBCHUNK_SIZE_OFFSET));
		printf("SubchunkID: %.4s\n", fileData+pos);
	}
	if(!(memcmp(fileData+pos, &"data", 4) == 0))
	    exit;
	return pos;
}

unsigned int readDataChunk(char* fileData, int fileLength, float** data)
{
	unsigned int pos = getPositionOfDataID(fileData, fileLength);
    *data = (float*) (fileData+pos+SUBCHUNK_DATA_OFFSET);
    return fileData+pos+SUBCHUNK_SIZE_OFFSET;
}

//modification
void schneller(char* path, float *data, int length, wavheader wave)
{
	int halfLength = length / 2;
	float halfOfData[halfLength];
	for(int i = 0; i < halfLength; i++)
		halfOfData[i] = data[i*2];
	writePCM(path, halfOfData, halfLength, wave);
}

//main
int main()
{
	char* fileData;
	int fileLength = readFile("test.wav", &fileData);
	wavheader wave = *((wavheader*) fileData);

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
	int length = readDataChunk(fileData, fileLength, &data);

    printf("readDataSuccess\n");
    writePCM("testClone.wav", data, length/4, wave);
	printf("writePCM done\n");
    schneller("testFaster.wav", data, length/4, wave);
	printf("schneller done\n");

	free(fileData);
	printf("done\n");
}
