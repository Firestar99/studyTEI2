#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "FT.h"


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

float* sinusSignal(int N, float f, float a, float r)
{
	float* signal = malloc(sizeof(float)*N);
	for(int n = 0; n < N; n++) {
		signal[n] = a*sin(f*2*M_PI*n/r);
	}
	return signal;
}

//main
int main()
{

    float* signal = sinusSignal(50, 1, 1, 50);
	float* signal2 = sinusSignal(50, 5, 2, 50);
	for(int i = 0; i<50; i++)
		signal[i] = signal[i] + signal2[i];
	plot(signal, 50);
	struct SPEKTRUM* spek = DFT(signal, 50);
	plot(spek->ampl, 50);

	free(signal);
	free(signal2);
	printf("done\n");
}
