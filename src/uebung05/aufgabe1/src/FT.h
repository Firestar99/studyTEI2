#include <stdlib.h>

struct SPEKTRUM {
  float* real;
  float* imag;
  float* ampl;
};
typedef struct SPEKTRUM spektrum;

#define M_PI 3.1415926535897932384626433832795028841971

spektrum* DFT(float* x, size_t N);

float* invDFT(spektrum* sp, size_t N);

void writeArray(char* name, float *X, int N, float f0);

void plotfile(char* name, int maxN);

#define plotnf(x,N,n,f0)        \
  {                             \
    writeArray(#x, x, N, f0);   \
    plotfile(#x, n);            \
  }

#define plotn(x,N,n) plotnf(x,N,n,0)

#define plot(x,N) plotn(x,N,0)

