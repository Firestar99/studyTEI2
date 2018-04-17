#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wave.h"
#define TRUE 1
#define FALSE 0
union charArrayToInt {
    char c[4];
    unsigned int i;
};

union charToFloat {
    char c[4];
    float f;
}

setStringFlag(char *string, int pos)
{
    string[pos] = '\0';
}

unsigned char *getFormat()
{
    FILE *wave;
    unsigned char *format;
    format = (unsigned char *)malloc(sizeof(unsigned char[5]));
    setStringFlag(format, 4);
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in getFormat\n");
    }
    else if (fseek(wave, 8, 0) != 0)
    {
        printf("Fehler beim positionieren des Zeigers in getFormat");
    }
    else if (fread(format, sizeof(unsigned char), 4, wave) < 4)
    {
        printf("Fehler beim lesen der Datei in getFormat");
    }
    fclose(wave);
    return format;
}

short getChannels()
{
    FILE *wave;
    short channels;
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in getChannels\n");
    }
    else if (fseek(wave, 22, 0) != 0)
    {
        printf("Fehler beim positionieren des Zeigers in getChannels");
    }
    else if (fread(&channels, sizeof(short), 1, wave) < 1)
    {
        printf("Fehler beim lesen der Datei in getChannels");
    }
    fclose(wave);
    return channels;
}

unsigned int getSampleRate()
{
    FILE *wave;
    unsigned int sampleRate;
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in getSampleRate\n");
    }
    else if (fseek(wave, 24, 0) != 0)
    {
        printf("Fehler beim positionieren des Zeigers in getSampleRate");
    }
    else if (fread(&sampleRate, sizeof(unsigned int), 1, wave) < 1)
    {
        printf("Fehler beim lesen der Datei in getSampleRate");
    }
    fclose(wave);
    return sampleRate;
}

chunkheader getHeader(int offset)
{
    FILE *wave;
    char headChar[8];
    union charArrayToInt size;
    struct CHUNKHEADER head;
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in getHeader\n");
    }
    else if (offset != 0 && fseek(wave, offset, 0) != 0)
    {
        printf("Fehler beim positionieren des Zeigers in getHeader\n");
    }
    else if (fread(headChar, sizeof(char), 8, wave) < 8)
    {
        printf("Fehler beim lesen der Datei in getHeader\n");
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            head.chunk_id[i] = headChar[i];
        }
        // writing backwards into the size Array because of Endian
        for (int i = 4; i < 8; i++)
        {
            size.c[i - 4] = headChar[i];
        }
        head.chunk_size = size.i;
    }
    fclose(wave);
    return head;
}

unsigned short getBitsPerSample()
{
    FILE *wave;
    unsigned short bitsPerSample;
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in getBitsPerSample\n");
    }
    else if (fseek(wave, 34, 0) != 0)
    {
        printf("Fehler beim positionieren des Zeigers in getBitsPerSample");
    }
    else if (fread(&bitsPerSample, sizeof(unsigned short), 1, wave) < 1)
    {
        printf("Fehler beim lesen der Datei in getBitsPerSample");
    }
    fclose(wave);
    return bitsPerSample;
}

int findDataAndGetPositionOfLength()
{
    FILE *wave;
    int fieldIndex = 0;
    char chunkID[5];
    setStringFlag(chunkID, 4);
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in readDataChumk\n");
    }
    do
    {
        if (fseek(wave, fieldIndex, 0) != 0)
        {
            printf("Fehler beim positionieren des Zeigers während der Suche des Data Chunk in readDataChunk");
        }
        else if (fread(chunkID, sizeof(char), 4, wave) < 4)
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

void getLengthOfData(int *lengthp, int pos)
{
    FILE *wave;
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in readDataChumk\n");
    }
    else if (fseek(wave, pos, 0) != 0)
    {
        printf("Fehler beim positionieren des Zeigers auf dataSize in readDataChunk");
    }
    else if (fread(lengthp, sizeof(char), 4, wave) < 1)
    {
        printf("Fehler beim lesen der Länge in readDataChunk");
    }

    fclose(wave);
    *lengthp = *lengthp / 4;
    printf("Länge der Daten %i", *lengthp);
}

float *readDataChunk(int *lengthp)
{
    FILE *wave;
    float *data;
    int fieldIndex;
    union charToFloat charFloat;
    char field[4];
    int dataIndex = 0;
    fieldIndex = findDataAndGetPositionOfLength();
    getLengthOfData(lengthp, fieldIndex);

    //writing the data into the float array
    data = (float *)malloc((*lengthp));
    if ((wave = fopen("test.wav", "r")) == NULL)
    {
        printf("Fehler beim öffnen der Datei in readDataChumk\n");
    }
    do
    {
        fieldIndex = fieldIndex + 4;
        if (fseek(wave, fieldIndex, 0) != 0)
        {
            printf("Fehler beim positionieren des Zeigers über die Daten in readDataChunk");
        }
        else if (fread(field, sizeof(char), 4, wave) < 1)
        {
            printf("Fehler beim lesen über die Daten in readDataChunk");
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                charFloat.c[i] = field[i];
            }

            data[dataIndex] = charFloat.f;
            dataIndex++;
        }
    } while (dataIndex < *lengthp);
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

int main()
{
    wavheader wave;
    FILE *test;
    unsigned char *format;
    int *dataLengthP;
    float *data;
    char riffHeaderString[5];
    char fmtHeaderString[5];

    setStringFlag(riffHeaderString, 4);
    setStringFlag(fmtHeaderString, 4);

    //get headers
    wave.riff_chunk_header = getHeader(0);
    wave.fmt_chunk_header = getHeader(12);

    //copy in a IDs into a String
    strncat(riffHeaderString, wave.riff_chunk_header.chunk_id, 4);
    strncat(fmtHeaderString, wave.fmt_chunk_header.chunk_id, 4);

    //print fields of the chunkheaders
    printf("riff ID: %s\nfmt ID: %s\n", riffHeaderString, fmtHeaderString);
    printf("riff Size: %i\n", wave.riff_chunk_header.chunk_size);
    printf("fmt Size: %i\n", wave.fmt_chunk_header.chunk_size);

    format = getFormat();
    //copy the String format into the riff_format field of wave
    strncat(wave.riff_format, format, 4);
    printf("riff Format: %s\n", format);

    wave.channels = getChannels();
    printf("Channels: %i\n", wave.channels);

    wave.sample_rate = getSampleRate();
    printf("Sample Rate: %u\n", wave.sample_rate);

    wave.bits_per_sample = getBitsPerSample();
    printf("bits per sample: %u\n", wave.bits_per_sample);

    wave.byterate = wave.sample_rate * wave.channels * wave.bits_per_sample;
    printf("Audiolänge in Sekunden: %f\n", (float)wave.riff_chunk_header.chunk_size / (float)(wave.byterate / 8));
    wave.format_type = 3;
    wave.block_align = wave.channels * wave.bits_per_sample / 8;

    data = readDataChunk(dataLengthP);
    writePCM("testClone.wav", data, *dataLengthP, wave);

    schneller(data, *dataLengthP, wave);
    free(format);
    free(data);
}
