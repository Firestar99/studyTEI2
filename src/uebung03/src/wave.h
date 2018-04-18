// Quelle des Orignals: http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/
// Änderungen:
// - Datentypen denen des Dateiformats angepasst
// - struct CHUNKHEADER eingeführt
// - HEADER liest nur den chunk "fmt "

// 
struct CHUNKHEADER {
  char chunk_id[4];                    // chunk id string
  unsigned int chunk_size;             // length of chunk
};
typedef struct CHUNKHEADER chunkheader;

// WAVE file header format
struct HEADER {
  //"RIFF" chunk
  chunkheader riff_chunk_header;       // RIFF string and *overall size* of file in bytes
  unsigned char riff_format[4];        // WAVE string

  //"fmt " chunk
  chunkheader fmt_chunk_header;      // fmt string with trailing null char, length = 16
  unsigned short format_type;          // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
  unsigned short channels;             // no.of channels
  unsigned int sample_rate;            // sampling rate (blocks per second)
  unsigned int byterate;               // SampleRate * NumChannels * BitsPerSample/8
  unsigned short block_align;          // NumChannels * BitsPerSample/8
  unsigned short bits_per_sample;      // bits per sample, 8- 8bits, 16- 16 bits etc
};

typedef struct HEADER wavheader;

void writePCM(char * name, float *signal, int N, wavheader header);
