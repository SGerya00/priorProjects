#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//checks whether the file is base64 or not (-1 if not)
int check_the_base64(const unsigned char *conv_list, FILE *input) {
    int belongs_to_base64 = 0; //flag
    int current_char_code = 0;
    size_t what_if = 0;
    while (EOF != (current_char_code = fgetc(input))) {
        belongs_to_base64 = 0;
        unsigned char current_char = (unsigned char)current_char_code;
        for (size_t i = 0; i < 64; i++) {
            if (current_char == conv_list[i]) {
                belongs_to_base64 = 1;
                i = 64;
            }
        }
        if (0 == belongs_to_base64) {
            printf("\n%c does not belong to base64 alphabet\n", current_char);
            return -1;
        }
        what_if += 1;
        if (4 == what_if) {
            what_if = 0;
        }
    }
    if (1 == what_if) {
        printf("\nBase64 encoded file contains 4*n + 1 character, "
               "which is impossible in Base64 encoded file\n");
        return -1;
    }
    return 0;
}
//creates a new file with ".orig" added, opens it and returns pointer to that open file
FILE * new_file_creator(const char *filename) {
    const char *input_filename = filename;
    size_t len_of_filename = strlen(input_filename);
    printf("%s\n", input_filename); //input file
    char *add = ".orig";
    size_t add_size = strlen(add);
    char * new_filename = (char*)calloc(len_of_filename + add_size + 1, sizeof(*new_filename));
    strcpy(new_filename, input_filename);
    for (int i = 0; i < add_size; i++) {
        new_filename[len_of_filename + i] = add[i];
    } //add *add
    new_filename[len_of_filename + add_size] = '\0';
    printf("%s\n", new_filename); //output file
    FILE *f = fopen(new_filename, "wb");
    if (NULL == f) {
        fprintf(stderr, "\nUnable to open freshly created file %s\n", new_filename);
        free(new_filename);
        return NULL;
    }
    free(new_filename);
    return f;
}
//to use in convert_and_print_in_file
void quartet_is_true(FILE *output, const unsigned char *qtet_char,\
                     size_t local_quartet_counter) {
    if (2 <= local_quartet_counter) {
        //(00abcdef -> abcdef00 & 11111100) | (00abcdef -> 000000ab & 00000011) = abcdef ab
        unsigned char a1 = (((qtet_char[0] << 2) & (0x3F << 2)) | ((qtet_char[1] >> 4) & (0x30 >> 4)));
        if (3 <= local_quartet_counter) {
            //(00abcdef -> cdef0000 & 11111100) | (00abcdef -> 0000abcd & 00001111) = cdef abcd
            unsigned char a2 = (((qtet_char[1] << 4) & (0x3F << 2)) | ((qtet_char[2] >> 2) & (0x3F >> 2)));
            if (4 == local_quartet_counter) {
                //(00abcdef -> ef000000 & 11000000) | (00abcdef & 00111111) = ef abcdef
                unsigned char a3 = (((qtet_char[2] << 6) & (0x30 << 2)) | (qtet_char[3] & 0x3F));
                fprintf(output, "%c%c%c", a1, a2, a3);
                return;
            }
            else {
                fprintf(output, "%c%c", a1, a2);
                return;
            }
        }
        else {
            fprintf(output, "%c", a1);
            return;
        }
    }
}
void convert_and_print_in_file(const unsigned char *conv_list, FILE *input, FILE *output) {
    unsigned char quartet_char[4] = {0};
    int truly_empty = 1; //to check if the file is empty (flag)
    size_t local_quartet_counter = 0;
    int char_code = 0;
    //getting 4 base64 characters, then converting them into 3 ASCII characters
    while (EOF != (char_code = fgetc(input))) {
        truly_empty = 0;
        //the character that was just read with fgetc
        unsigned char char_code_c = (unsigned char)char_code;
        size_t read_ch_code_64;
        //here we get the position of the read character in conversion list
        for (size_t i = 0; i < 64; i++) {
            if (char_code_c == conv_list[i]) {
                read_ch_code_64 = i;
                i = 64;
            }
        }
        quartet_char[local_quartet_counter] = (unsigned char)read_ch_code_64;
        local_quartet_counter += 1;
        if (4 == local_quartet_counter) {
            //converting from base64
            quartet_is_true(output, quartet_char, local_quartet_counter);
            local_quartet_counter = 0;
        }
    }
    if (1 == truly_empty) {
        printf("\nThe file is empty\n");
        return;
    }
    //now we have to check how many symbols are left after exhausting all the quartets
    if (0 == local_quartet_counter) {
        return;
    }
    //local_quartet_counter cannot be 1
    if (2 == local_quartet_counter) {
        quartet_is_true(output, quartet_char, local_quartet_counter);
        return;
    }
    if (3 == local_quartet_counter) {
        quartet_is_true(output, quartet_char, local_quartet_counter);
        return;
    }
}
int main(int argc, char *argv[]) {
    if (2 != argc) {
        fprintf(stderr, "\nUnexpected amount of arguments: %d\n", argc);
        return -1;
    }
    unsigned char from_64_list[64] = {'A','B','C','D','E','F','G',\
    'H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W',\
    'X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m',\
    'n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2',\
    '3','4','5','6','7','8','9','+','/'};
    FILE *input = fopen(argv[1], "rb"); //input is open
    if (NULL == input) {
        printf("Unable to open file\n");
        return -1;
    }
    if (-1 == check_the_base64(from_64_list, input)) {
        printf("The file is not a Base64 encoded file\n");
        fclose(input);
        return -1;
    }
    //after check_the_base64 we are at the EOF, need to go to the start of file
    fclose(input); //input is closed
    input = fopen(argv[1], "rb");
    if (NULL == input) {
        printf("Unable to open file\n");
        return -1;
    }
    FILE *output = new_file_creator(argv[1]);
    if (NULL == output) {
        fclose(input);
        return -1;
    }
    convert_and_print_in_file(from_64_list, input, output);
    fclose(input);
    fclose(output);
    printf("\nDone\n");
    return 0;
}