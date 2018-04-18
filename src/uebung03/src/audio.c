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

//utility
union charArrayToInt {
	char c[4];
	unsigned int i;
};

union charToFloat {
	char c[4];
	float f;
}

setNullTerminator(char *string, int length)
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

int findDataKeyword()
{
	printf("h1\n");
	FILE* wave;
	int fieldIndex = 0;
	char chunkID[5];
	setNullTerminator(chunkID, 5);
	if ((wave = fopen("test.wav", "r")) == NULL)
	{
		printf("Fehler beim öffnen der Datei in readDataChumk\n");
	}
	printf("h2\n");
	do
	{
		if (fseek(wave, fieldIndex, 0) != 0)
		{
			printf("Fehler beim positionieren des Zeigers während der Suche des Data Chunk in readDataChunk");
		}
		else if (fread(&chunkID, sizeof(char), 4, wave) < 4)
		{
			printf("Fehler beim lesen eines Feldes während der Suche des Data Chunk in readDataChunk");
		}
		//	for debugging purposes
		//  printf("%i\n %s\n\n", fieldIndex, chunkID);

		fieldIndex = fieldIndex + 4;
	} while (strcmp(chunkID, "data") != 0);

	fclose(wave);
	return fieldIndex;
}

void getLengthOfData(int* lengthp, int pos)
{
	printf("g0\n");
	FILE *wave;
	if ((wave = fopen("test.wav", "r")) == NULL)
	{
		printf("Fehler beim öffnen der Datei in readDataChumk\n");
		return;
	}
	printf("g1 %X\n", pos);
	if (fseek(wave, pos, 0) != 0)
	{
		printf("Fehler beim positionieren des Zeigers auf dataSize in readDataChunk");
		return;
	}
	printf("g2\n");
	if (fread(lengthp, sizeof(int), 1, wave) != 4)
	{
		printf("Fehler beim lesen der Länge in readDataChunk");
		return;
	}
	printf("g3\n");

	fclose(wave);
	*lengthp = *lengthp / 4;
	printf("Länge der Daten %i", *lengthp);
}

float* readDataChunk(int *lengthp)
{
	FILE *wave;
	float *data;
	int fieldIndex;
//    union charToFloat charFloat;
//    char field[4];
//    int dataIndex = 0;
	printf("p1\n");
	fieldIndex = findDataKeyword();
	printf("p2 %X\n", fieldIndex);
	getLengthOfData(lengthp, fieldIndex);
	printf("p3\n");

	//writing the data into the float array
//    data = (float *)malloc((*lengthp));
	if ((wave = fopen("test.wav", "r")) == NULL)
	{
		printf("Fehler beim öffnen der Datei in readDataChumk\n");
	}
//    do
//    {
//        fieldIndex = fieldIndex + 4;
//        if (fseek(wave, fieldIndex, 0) != 0)
//        {
//            printf("Fehler beim positionieren des Zeigers über die Daten in readDataChunk");
//        }
//        else if (fread(field, sizeof(char), 4, wave) < 1)
//        {
//            printf("Fehler beim lesen über die Daten in readDataChunk");
//        }
//        else
//        {
//            for (int i = 0; i < 4; i++)
//            {
//                charFloat.c[i] = field[i];
//            }
//
//            data[dataIndex] = charFloat.f;
//            dataIndex++;
//        }
//    } while (dataIndex < *lengthp);
	fclose(wave);
	return data;
}

void schneller(float *data, int length, wavheader wave)
{
	int halfLength = length / 2;
	float halfOfData[halfLength];
	int insert = FALSE;
	for (int i = 0; i < halfLength; i++)
	{
		if (insert == TRUE)
		{
			halfOfData[i] = data[i];
			insert = FALSE;
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

	wave.bits_per_sample = getBitsPerSample(fileData, fileLength);
	printf("bits per sample: %u\n", wave.bits_per_sample);

	wave.byterate = wave.sample_rate * wave.channels * wave.bits_per_sample;
	printf("Audiolänge in Sekunden: %f\n", (float)wave.riff_chunk_header.chunk_size / (float)(wave.byterate / 8));
	wave.format_type = 3;
	wave.block_align = wave.channels * wave.bits_per_sample / 8;

//	printf("point1\n");
//	int* dataLengthP;
//	float* data = readDataChunk(dataLengthP);
//	printf("point2\n");
////    writePCM("testClone.wav", data, *dataLengthP, wave);
//	printf("point3\n");
//
////    schneller(data, *dataLengthP, wave);
//	printf("point4\n");
//    free(format);
//    free(data);

	free(fileData);
	printf("done\n");
}
