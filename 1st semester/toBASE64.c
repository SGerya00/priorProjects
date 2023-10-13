#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//creates a new file with ".base64" added, opens it and returns pointer to that open file
FILE * new_file_creator(const char *filename) {
    const char *input_filename = filename;
    size_t len_of_filename = strlen(input_filename);
    printf("%s\n", input_filename); //input file
    char *add = ".base64";
    size_t add_size = strlen(add);
    char * new_filename = (char*)calloc(len_of_filename + add_size + 1, sizeof(*new_filename));
    strcpy(new_filename, input_filename); //instead of 4 lines of code
    for (int i = 0; i < add_size; i++) {
        new_filename[len_of_filename + i] = add[i];
    } //add *add
    new_filename[len_of_filename + add_size] = '\0';
    printf("%s\n", new_filename); //output file
    FILE *f = fopen(new_filename, "wb");
    if (NULL == f) {
        fprintf(stderr, "\nDid not succeed to open freshly created file %s\n", new_filename);
        free(new_filename);
        return NULL;
    }
    free(new_filename);
    return f;
}
//to use in convert_and_print_in_file
void triad_is_true(const unsigned char *conv_list, FILE *output,\
                   unsigned char *triad_char, size_t local_triad_counter) {
    if (1 <= local_triad_counter) {
        unsigned char a1 = ((triad_char[0] >> 2) & 0x3F);
        int i_a1 = (int)a1;
        unsigned char c_a1 = conv_list[i_a1];
        if (2 <= local_triad_counter) {
            unsigned char a2 = (((triad_char[0] << 4) & 0x30) | ((triad_char[1] >> 4) & (0x3F >> 2)));
            int i_a2 = (int)a2;
            unsigned char c_a2 = conv_list[i_a2];
            if (3 == local_triad_counter) {
                unsigned char a3 = (((triad_char[1] << 2) & 0x3F) | (triad_char[2] >> 6));
                int i_a3 = (int)a3;
                unsigned char c_a3 = conv_list[i_a3];
                unsigned char a4 = (triad_char[2] & 0x3F);
                int i_a4 = (int)a4;
                unsigned char c_a4 = conv_list[i_a4];
                fprintf(output, "%c%c%c%c", c_a1, c_a2, c_a3, c_a4);
                return;
            }
            else {
                unsigned char a3 = ((triad_char[1] << 2) & 0x3F);
                int i_a3 = (int)a3;
                unsigned char c_a3 = conv_list[i_a3];
                fprintf(output, "%c%c%c", c_a1, c_a2, c_a3);
                return ;
            }
        }
        else {
            unsigned char a2 = ((triad_char[0] << 4) & 0x30);
            int i_a2 = (int)a2;
            unsigned char c_a2 = conv_list[i_a2];
            fprintf(output, "%c%c", c_a1, c_a2);
            return;
        }
    }
}
void convert_and_print_in_file(const unsigned char *conv_list, FILE *input, FILE *output) {
    unsigned char triad_char[3] = {0};
    int truly_empty = 1; //to check if the file is empty (flag)
    size_t local_triad_counter = 0;
    int char_code = 0;
    //getting 3 characters, then converting them into 4 base64 characters
    while (EOF != (char_code = fgetc(input))) {
        truly_empty = 0;
        triad_char[local_triad_counter] = (unsigned char)char_code;
        local_triad_counter += 1;
        if (3 == local_triad_counter) {
            //converting to base64
            triad_is_true(conv_list, output, triad_char, local_triad_counter);
            local_triad_counter = 0;
        }
    }
    if (1 == truly_empty) {
        printf("\nThe file is empty\n");
        return;
    }
    //now we have to check how many symbols are left after exhausting all the triads
    if (0 == local_triad_counter) {
        return;
    }
    if (1 == local_triad_counter) {
        triad_is_true(conv_list, output, triad_char, local_triad_counter);
        return;
    }
    if (2 == local_triad_counter) {
        triad_is_true(conv_list, output, triad_char, local_triad_counter);
        return;
    }
}
int main(int argc, char *argv[]) {
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected amount of arguments: %d\n", argc);
        return -1;
    }
    unsigned char to_64_list[64] = {'A','B','C','D','E','F','G','H',\
    'I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X',\
    'Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n',\
    'o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3',\
    '4','5','6','7','8','9','+','/'};
    FILE *input = fopen(argv[1], "rb");
    if (NULL == input) {
        fprintf(stderr, "Unable to open input file %s in *main*\n", argv[1]);
        return -1;
    }
    FILE *output = new_file_creator(argv[1]);
    if (NULL == output) {
        fclose(input);
        return -1;
    }
    convert_and_print_in_file(to_64_list, input, output);
    fclose(input);
    fclose(output);
    printf("\nDone\n");
    return 0;
}