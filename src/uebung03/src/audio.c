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

//header info
chunkheader getHeader(char* fileData, int fileLength, int offset)
{
	struct CHUNKHEADER head;
	for (int i = 0; i < 4; i++)
		head.chunk_id[i] = fileData[offset+i];

	// writing backwards into the size Array because of Endian
	union charArrayToInt size;
	for (int i = 4; i < 8; i++)
		size.c[i - 4] = fileData[offset+i];
	head.chunk_size = size.i;

	return head;
}

unsigned char* getFormat(char* fileData, int fileLength)
{
	unsigned char* format = (unsigned char*) malloc(sizeof(unsigned char[FORMAT_LENGTH+1]));
	memcpy(format, fileData+FORMAT_OFFSET, FORMAT_LENGTH);
	setNullTerminator(format, FORMAT_LENGTH+1);
	return format;
}

//audio info
short getChannels(char* fileData, int fileLength)
{
	return *(fileData+CHANNEL_OFFSET);
}

unsigned int getSampleRate(char* fileData, int fileLength)
{
	return *(fileData+SAMPLE_RATE_OFFSET);
}

unsigned short getBitsPerSample(char* fileData, int fileLength)
{
	return *(fileData+BITS_PER_SAMPLE_OFFSET);
}

unsigned int getPositionOfDataID(char* fileData, int fileLength)
{
	int position = FMT_HEADER_OFFSET;
	char chunkID[5];
	setNullTerminator(chunkID, FIELD_LENGTH + 1);
	while(position < fileLength && strcmp(chunkID, "data") != 0)
	{
	    position += DISTANCE_ID_TO_DATA + (*(fileData+position+DISTANCE_ID_TO_SIZE));
		memcpy(chunkID, fileData+position, FIELD_LENGTH);
		printf("SubchunkID: %s\n", chunkID);
	}
	if(!(strcmp(chunkID, "data") == 0))
	{
	    exit;
	}
	return position;
}

void getLengthOfData(char* fileData, int position, int* lengthOfData)
{
	union charArrayToInt size;
    for (int i = 0; i < 4; i++)
   		size.c[i] = *(fileData+position+DISTANCE_ID_TO_SIZE+i);
   	*lengthOfData = size.i;
}

float* getFloatArrayOfData(char* fileData, int position, int* lengthOfData)
{
	float* data = malloc(*lengthOfData);
	position += DISTANCE_ID_TO_DATA;
	for(int i = 0; position < (*lengthOfData/4); i++)
	{
//		union charArrayToInt field;
//		for (int i = 0; i < 4; i++)
//           	field.c[(i)] = *(fileData+position+i);
//        data[i] = field.i;
        data[i] = *(fileData+position);
        position += FIELD_LENGTH;

	}
	return data;
}

float* readDataChunk(char* fileData, int fileLength ,int *lengthOfData)
{
    int position = getPositionOfDataID(fileData, fileLength);
    printf("position: %i\n", position);
    getLengthOfData(fileData, position, lengthOfData);
    printf("lengthOfData: %i\n", *lengthOfData);
    return getFloatArrayOfData(fileData, position, lengthOfData);

}


void schneller(float *data, int length, wavheader wave)
{
	int halfLength = length / 2;
	float halfOfData[halfLength];
	int insert = FALSE;
	for (int i = 0, j = 0; j < halfLength; i++)
	{
		if (insert == TRUE)
		{
			halfOfData[j] = data[i];
			insert = FALSE;
			j++;
		}
		else
		{
			insert = TRUE;
		}
	}
	writePCM("testFaster.wav", halfOfData, halfLength, wave);
}

int readFile(char* name, char** data) {
	FILE* wave;
	if((wave = fopen(name, "r")) == NULL)
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
	//get headers
	wavheader wave;
	wave.riff_chunk_header = getHeader(fileData, fileLength, RIFF_HEADER_OFFSET);
	wave.fmt_chunk_header = getHeader(fileData, fileLength, FMT_HEADER_OFFSET);

	//copy in a IDs into a String
	char riffHeaderString[RIFF_HEADER_LENGTH+1];
	memcpy(&riffHeaderString, &wave.riff_chunk_header.chunk_id, RIFF_HEADER_LENGTH);
	setNullTerminator(riffHeaderString, RIFF_HEADER_LENGTH+1);
	char fmtHeaderString[FMT_HEADER_LENGTH+1];
	memcpy(&fmtHeaderString, &wave.fmt_chunk_header.chunk_id, FMT_HEADER_LENGTH);
	setNullTerminator(fmtHeaderString, FMT_HEADER_LENGTH+1);

	//print fields of the chunkheaders
	printf("RIFF ID: %s\n", riffHeaderString);
	printf("RIFF size: %i\n", wave.riff_chunk_header.chunk_size);
	printf("fmt ID: %s\n", fmtHeaderString);
	printf("fmt size: %i\n", wave.fmt_chunk_header.chunk_size);

	unsigned char* format = getFormat(fileData, fileLength);
	//copy the String format into the riff_format field of wave
	strncat(wave.riff_format, format, 4);
	printf("riff Format: %s\n", format);

	wave.channels = getChannels(fileData, fileLength);
	printf("Channels: %i\n", wave.channels);

	wave.sample_rate = getSampleRate(fileData, fileLength);
	printf("Sample Rate: %u\n", wave.sample_rate);
//	wave.sample_rate = 8000;
	wave.bits_per_sample = getBitsPerSample(fileData, fileLength);
	printf("bits per sample: %u\n", wave.bits_per_sample);

	wave.byterate = wave.sample_rate * wave.channels * wave.bits_per_sample/8;
	printf("Audiolänge in Sekunden: %f\n", (float)wave.riff_chunk_header.chunk_size / (float)(wave.byterate / 8));
	wave.format_type = 3;
	wave.block_align = wave.channels * wave.bits_per_sample / 8;

    int length;
    int *lengthOfData = &length;
	float* data = readDataChunk(fileData, fileLength, lengthOfData);
    printf("readDataSuccess\n");
    writePCM("testClone.wav", data, (*lengthOfData/4), wave);
	printf("writePCM done\n");
    schneller(data, (*lengthOfData/4), wave);
	printf("schneller done\n");

    free(data);
	free(fileData);
	printf("done\n");
}
