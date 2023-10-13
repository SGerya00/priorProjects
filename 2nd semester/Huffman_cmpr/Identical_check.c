#include <stdio.h>
#include <assert.h>

int check_2_files(FILE *file_one, FILE *file_two) {
    assert((NULL != file_one) && (NULL != file_two));

    int char_one = 0;
    int char_two = 0;
    unsigned long long int counter = 0;
    while(1) {
        char_one = fgetc(file_one);
        char_two = fgetc(file_two);
        if (char_one != char_two) {
            fprintf(stdout, "\nFiles are not identical, difference in %llu symbol (%c in 1st, %c in 2nd)\n",
                    counter, (char)char_one, (char)char_two);
            return 0;
        }
        counter += 1;
        if (EOF == char_one) {
            fprintf(stdout, "\nEnd Of File 1 reached\n");
            break;
        }
    }
    if (char_one == char_two) {
        fprintf(stdout, "\nFiles are indeed identical\n");
        return 1;
    }
    return 0; //assert(NULL)
}

int main_check(FILE *file1, FILE *file2) {
    assert((NULL != file1) && (NULL != file2));
    fprintf(stdout, "\nCommencing file comparison...");
    int result = check_2_files(file1, file2);
    if (1 == result) {
        fprintf(stdout, "\nSuccess\n");
    } else {
        fprintf(stdout, "\nNot successful\n");
    }
    return 0;
}
