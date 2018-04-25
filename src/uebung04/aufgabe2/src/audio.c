#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wave.h"


const int FMT_HEADER_OFFSET = 12;
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
	}
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

float* sinusSignal(int N, float f, float a, float r)
{
    float* signal = malloc(sizeof(float)*N);
    for(int n = 0; n < N; n++) {
        signal[n] = a*sin(f*2*M_PI*n/r);
    }
    return signal;
}

float getAverage(float* data, int length)
{
	float average = 0;
    for(int i = 0; i < length; i++)
        average += fabs(data[i]);
    average = average/length;
    return average;
}

float* mix(float* signal1, float* signal2, int length)
{
    float* mix = malloc(sizeof(float)*length);
    for(int i = 0; i < length; i++)
        mix[i] = (signal1[i]+signal2[i])/2;
    return mix;
}

void dumpPrintHex(char* p, unsigned int l, unsigned int pos)
{
	if(pos > l)
		printf("  ");
	else
		printf("%02hhX", p[pos]);
}

void dumpPrintChar(char* p, unsigned int l, unsigned int pos)
{
	if(pos > l)
		printf(" ");
	else
		printf("%c", p[pos]);
}

void dump(char* p, unsigned int l)
{
	for(unsigned int i = 0; i < l+16; i += 16)
	{
		printf("%08u ", i);
		for(unsigned int j = 0; j < 16; j += 2)
		{
			dumpPrintHex(p, l, i+j);
			dumpPrintHex(p, l, i+j+1);
			printf(" ");
		}
		printf(" - ");
		for(unsigned int j = 0; j < 16; j += 2)
		{
			dumpPrintChar(p, l, i+j);
			dumpPrintChar(p, l, i+j+1);
			printf(" ");
		}
		printf("\n");
	}
}

//main
int main()
{
	char* fileData;
	int fileLength = readFile("test.wav", &fileData);
	wavheader wave = *((wavheader*) fileData);

	float* data;
	int length = readDataChunk(fileData, fileLength, &data) / 4;

	float average = getAverage(data, length);

    float* signal = sinusSignal(length, 1600, average, wave.sample_rate);
    //dump(signal, length*4);
    writePCM("sinus1600Hz", signal, length, wave);
    printf("creating sinus1600Hz done\n");

    float* signal2 = sinusSignal(length, 7200, average, wave.sample_rate);
    writePCM("sinus7200Hz", signal2, length, wave);
    printf("creating sinus7200Hz done\n");

    //signal 2 ist tiefer

    float* mixed = mix(data, signal, length);
    writePCM("mix", mixed, length, wave);
    printf("creating mix: done\n");

	free(fileData);
	free(signal);
	free(signal2);
	free(mixed);
	printf("done\n");
}
