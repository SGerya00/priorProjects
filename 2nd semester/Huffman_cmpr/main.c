#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "NVR_D1_B4_nec_defines.h"

#define KEY_FOR_ENCODE "encode"
#define KEY_FOR_DECODE "decode"
#define KEY_FOR_COMPARE "compare"

char *make_new_filename(char *filename, char *suffix) {
    assert(NULL != filename);

    size_t length = strlen(filename);
    size_t suff_len = strlen(suffix);
    char *new_name = (char *)calloc(length + suff_len + 1, sizeof(char)); //+1 for \0
    assert(NULL != new_name);
    memcpy(new_name, filename, length + 1);
    memcpy(new_name + (sizeof(char) * length), suffix, suff_len + 1);
    new_name[length + suff_len] = '\0';
    MY_LOG("\n   --> new filename is \"%s\"", new_name);
    return new_name;
} //memory is allocated, please free

int main(int argc, char *argv[]) {
    if ((3 > argc) || (4 < argc)) {
        goto abort_label;
    }
    if (0 == strcmp(KEY_FOR_ENCODE, argv[1])) {
        FILE *input_enc = fopen(argv[2], "rb");
        assert(NULL != input_enc);
        char *output_name = make_new_filename(argv[2], SUFFIX_ENC);
        FILE *output_enc = fopen(output_name, "wb");
        assert(NULL != output_enc);
        main_encoder2(input_enc, output_enc);
        MY_LOG("\nTrying to close output"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fclose(output_enc);
        MY_LOG("\nTrying to close input"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fclose(input_enc);
        free(output_name);
    } else if (0 == strcmp(KEY_FOR_DECODE, argv[1])) {
        FILE *input_dec = fopen(argv[2], "rb");
        assert(NULL != input_dec);
        char *output_name = make_new_filename(argv[2], SUFFIX_DEC);
        FILE *output_dec = fopen(output_name, "wb");
        assert(NULL != output_dec);
        main_decoder(input_dec, output_dec);
        MY_LOG("\nTrying to close output"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fclose(output_dec);
        MY_LOG("\nTrying to close input"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fclose(input_dec);
        free(output_name);
    } else if (0 == strcmp(KEY_FOR_COMPARE, argv[1])) {
        FILE *orig = fopen(argv[2], "rb");
        assert(NULL != orig);
        FILE *decoded = fopen(argv[3], "rb");
        assert(NULL != decoded);
        main_check(orig, decoded);
        MY_LOG("\nTrying to close output"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fclose(decoded);
        MY_LOG("\nTrying to close output"); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fclose(orig);
    } else {
        goto abort_label;
    }
    PROGRESS_LOG("\nDone (big done)");
    return 0;
    //
    abort_label:
    fprintf(stderr, "\nIncorrect input, expected \"encode *filename*\" "
                    "or \"decode *filename*\" or \"compare *filename* *filename*\"\n");
    return 0;
    //
}
