#ifndef HUFFMAN_COMPRESSION_NVR_D1_B4_NEC_DEFINES_H
#define HUFFMAN_COMPRESSION_NVR_D1_B4_NEC_DEFINES_H

#define MAX_ARR_SIZE 256
#define MAX_CODE_LEN 32 //bcs 32*8=256
#define USED 1
#define NOT_USED 0
#define ONE_ZERO (unsigned char)0x80
#define ZERO_ZERO (unsigned char)0x00
#define SUFFIX_ENC ".output"
#define SUFFIX_DEC ".orig"

#define PROGRESS_LOG(...) fprintf(stdout, __VA_ARGS__)

//#define LOGGING_ON
#ifdef LOGGING_ON
#define MY_LOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define MY_LOG(...)
#endif
//
int main_encoder2(FILE *, FILE *);
int main_decoder(FILE *, FILE *);
int main_check(FILE *, FILE *);
//
#endif //HUFFMAN_COMPRESSION_NVR_D1_B4_NEC_DEFINES_H
