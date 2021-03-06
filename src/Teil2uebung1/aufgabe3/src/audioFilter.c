#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wave.h"

const float TAB_VALUE = 0.05f;
const int TAB_COUNT = 20;

const int FMT_HEADER_OFFSET = 12;
const int SUBCHUNK_SIZE_OFFSET = 4;
const int SUBCHUNK_DATA_OFFSET = 8;

//read file
int readFile(char *name, char **data) {
    FILE *wave;
    if ((wave = fopen(name, "rb")) == NULL) {
        printf("Could not read File!");
        exit;
    }

    int seekRet = fseek(wave, 0, SEEK_END);
    if (seekRet != 0) {
        printf("fseek returned != 0");
        exit;
    }

    int length = ftell(wave);
    if (length == -1) {
        printf("ftell returned -1");
        exit;
    }
    rewind(wave);

    char *buffer = malloc(sizeof(char) * length);
    if (fread(buffer, sizeof(char), length, wave) != length) {
        printf("didn't read all the data!");
        exit;
    }

    fclose(wave);

    *data = buffer;
    return length;
}

//data chunk
unsigned int getPositionOfDataID(char *fileData, int fileLength) {
    unsigned int pos = FMT_HEADER_OFFSET;
    while (pos < fileLength && memcmp(fileData + pos, &"data", 4) != 0) {
        pos += SUBCHUNK_DATA_OFFSET + (*(fileData + pos + SUBCHUNK_SIZE_OFFSET));
    }
    if (memcmp(fileData + pos, &"data", 4) != 0)
        exit;
    return pos;
}

unsigned int readDataChunk(char *fileData, int fileLength, float **data) {
    unsigned int pos = getPositionOfDataID(fileData, fileLength);
    *data = (float *) (fileData + pos + SUBCHUNK_DATA_OFFSET);
    return *((unsigned int *) (fileData + pos + SUBCHUNK_SIZE_OFFSET));
}

void generateNewTabs(float *newTabs, int i, float *data, int length, double *tabs, int tabCount) {
    for (int k = 0; k < tabCount; k++) {
        float x = 0;
        for (int j = 0; j < 4; j++) {
            x += data[(i - j - 1 + length) % length] * ((float)tabs[j]);
        }
        newTabs[k] = ((float)tabs[k]) - 0.01f * data[(i - k - 1+ length) % length] * (x - data[i]);
    }
}

float *filter(float *data, int length, double *tabs, int tabCount) {
    float sum = 0;
    float *newTabs = malloc(sizeof(float) * tabCount);
    float *filter = malloc(sizeof(float) * length);
    for (int i = 0; i < length; i++) {
        generateNewTabs(newTabs, i, data, length, tabs, tabCount);
        for (int j = 0; j < tabCount; j++)
            sum += data[(i - j + length - 1) % length] * newTabs[j];
        filter[i] = sum;
        sum = 0;
    }
    free(newTabs);
    return filter;

}

int main() {
    char *fileData;
    int fileLength = readFile("test.wav", &fileData);
    wavheader wave = *((wavheader *) fileData);

    float *data;
    int length = readDataChunk(fileData, fileLength, &data) / 4;

    double *tabs = malloc(sizeof(double) * TAB_COUNT);
    for (int i = 0; i < TAB_COUNT; i++) {
        tabs[i] = TAB_VALUE;
    }
    float *predictedData = filter(data, length, tabs, TAB_COUNT);
    writePCM("predictionTest.wav", predictedData, length, wave);

    free(fileData);
    free(tabs);

    printf("done\n");
}
