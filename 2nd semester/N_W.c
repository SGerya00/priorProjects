#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//constraints
#define MAX_SEQUENCE_LENGTH 10000
#define GAP_PENALTY_INT -5
#define SCORE_TYPE int
//
#define ST(x,y) (*((*score_table)[(x)]))[(y)] //x - row, y - column
#define SM(x,y) (*((*similarity_matrix)[(x)]))[(y)]
//

//#define LOGGING_ON
#ifdef LOGGING_ON
#define MY_LOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define MY_LOG(...)
#endif


enum DNA_nucleotides {A, T, G, C};

void fill_similarity_matrix(SCORE_TYPE (*(*SM)[])[]) {
    assert(NULL != SM);

    (*(*SM)[A])[A] = 10;
    (*(*SM)[A])[T] = -4;
    (*(*SM)[A])[G] = -1;
    (*(*SM)[A])[C] = -3;
    (*(*SM)[T])[A] = -4;
    (*(*SM)[T])[T] = 8;
    (*(*SM)[T])[G] = -3;
    (*(*SM)[T])[C] = 0;
    (*(*SM)[G])[A] = -1;
    (*(*SM)[G])[T] = -3;
    (*(*SM)[G])[G] = 7;
    (*(*SM)[G])[C] = -5;
    (*(*SM)[C])[A] = -3;
    (*(*SM)[C])[T] = 0;
    (*(*SM)[C])[G] = -5;
    (*(*SM)[C])[C] = 9;

    // .|A  |  T |  G |  C |
    //--|---|----|----|--- |
    //A |10 | -4 | -1 | -3 |
    //--|---|----|----|--- |
    //T |-4 |  8 | -3 |  0 |
    //--|---|----|----|--- |
    //G |-1 | -3 |  7 | -5 |
    //--|---|----|----|--- |
    //C |-3 |  0 | -5 |  9 |
}

void fill_similarity_matrix_BLOSUM62(SCORE_TYPE (*(*SM)[])[]) {
    assert(NULL != SM);

    (*(*SM)[A])[A] = 4;
    (*(*SM)[A])[T] = 0;
    (*(*SM)[A])[G] = 0;
    (*(*SM)[A])[C] = 0;
    (*(*SM)[T])[A] = 0;
    (*(*SM)[T])[T] = 5;
    (*(*SM)[T])[G] = -2;
    (*(*SM)[T])[C] = -1;
    (*(*SM)[G])[A] = 0;
    (*(*SM)[G])[T] = -2;
    (*(*SM)[G])[G] = 6;
    (*(*SM)[G])[C] = -3;
    (*(*SM)[C])[A] = 0;
    (*(*SM)[C])[T] = -1;
    (*(*SM)[C])[G] = -3;
    (*(*SM)[C])[C] = 9;

    // .|A  |  T |  G |  C |
    //--|---|----|----|--- |
    //A | 4 |  0 |  0 |  0 |
    //--|---|----|----|--- |
    //T | 0 |  5 | -2 | -1 |
    //--|---|----|----|--- |
    //G | 0 | -2 |  6 | -3 |
    //--|---|----|----|--- |
    //C | 0 | -1 | -3 |  9 |
}

void fill_similarity_matrix_DNAfull(SCORE_TYPE (*(*SM)[])[]) {
    assert(NULL != SM);

    (*(*SM)[A])[A] = 5;
    (*(*SM)[A])[T] = -4;
    (*(*SM)[A])[G] = -4;
    (*(*SM)[A])[C] = -4;
    (*(*SM)[T])[A] = -4;
    (*(*SM)[T])[T] = 5;
    (*(*SM)[T])[G] = -4;
    (*(*SM)[T])[C] = -4;
    (*(*SM)[G])[A] = -4;
    (*(*SM)[G])[T] = -4;
    (*(*SM)[G])[G] = 5;
    (*(*SM)[G])[C] = -4;
    (*(*SM)[C])[A] = -4;
    (*(*SM)[C])[T] = -4;
    (*(*SM)[C])[G] = -4;
    (*(*SM)[C])[C] = 5;

    // .|A  |  T |  G |  C |
    //--|---|----|----|--- |
    //A | 5 | -4 | -4 | -4 |
    //--|---|----|----|--- |
    //T |-4 |  5 | -4 | -4 |
    //--|---|----|----|--- |
    //G |-4 | -4 |  5 | -4 |
    //--|---|----|----|--- |
    //C |-4 | -4 | -4 |  5 |
}

char * get_sequence(int t) {

    char *sequence = (char *)calloc(MAX_SEQUENCE_LENGTH + 1, sizeof(char)); //+1 for \0
    assert(NULL != sequence);
    fprintf(stdout, "\nPlease, input DNA sequence #%d: ",t);
    fgets(sequence, MAX_SEQUENCE_LENGTH + 1, stdin); //fgets read up to (n - 1) symbols
    sequence[MAX_SEQUENCE_LENGTH] = '\0'; //just in case

    return sequence;
} //memory is allocated

size_t get_sequence_size(const char *const sequence) {
    assert(NULL != sequence);

    size_t counter = 0;
    size_t index = 0;
    while(1) {
        if ((sequence[index] != 'A') &&
            (sequence[index] != 'T') &&
            (sequence[index] != 'G') &&
            (sequence[index] != 'C')) {
            break;
        }
        counter += 1;
        index += 1;
    }
    return counter;
}

void copy_sequence(const char *const sequence_1, char *const sequence_2, size_t size) {
    assert(NULL != sequence_1);
    assert(NULL != sequence_2);

    for (register size_t i = 0; i < size; i++) {
        sequence_2[i] = sequence_1[i];
    }
    sequence_2[size] = '\0';
}
//stops counting if encounters char that does not belong to DNA_nucleotides

//----------------------------------------------------------------------------------------------------------------------
void print_similarity_matrix(SCORE_TYPE (*(*SM)[])[]) {
    assert(NULL != SM);

    MY_LOG("\n");
    MY_LOG(".  A  T  G  C");
    for (register unsigned short int i = 0; i < 4; i++) {
        if (i == 0) {
            MY_LOG("\nA");
        } else if (i == 1) {
            MY_LOG("\nT");
        } else if (i == 2) {
            MY_LOG("\nG");
        } else {
            MY_LOG("\nC");
        }
        for (register unsigned short int j = 0; j < 4; j++) {
            MY_LOG(" %2d",  (*(*SM)[i])[j]    );
        }
    }
    MY_LOG("\n");
}

void print_score_table(SCORE_TYPE (*(*score_table)[])[], size_t size1, size_t size2) {
    assert(NULL != score_table);
    MY_LOG("\n\nThe score table is as follows:");
    for (register size_t i = 0; i <= size1; i++) {
        MY_LOG("\n\n");
        for (register size_t j = 0; j <= size2; j++) {
            MY_LOG(" %4d",   (*((*score_table)[i]))[j]    );
        }
    }
    MY_LOG("\n");
}
//----------------------------------------------------------------------------------------------------------------------

void print_sequence(const char *const sequence) {
    assert(NULL != sequence);

    MY_LOG("\nsequence is: ");
    size_t index = 0;
    while(1) {
        if (sequence[index] != '\0') {
            MY_LOG("%c", sequence[index]);
            index += 1;
        } else {
            break;
        }
    }
}

SCORE_TYPE (*(*create_similarity_matrix())[])[] {

    SCORE_TYPE (*(*similarity_matrix)[])[] = (SCORE_TYPE (*(*)[])[])
                                              calloc(4, sizeof(SCORE_TYPE (*)[]));
    assert(NULL != similarity_matrix);
    (*similarity_matrix)[0] = (SCORE_TYPE (*)[])calloc(4, sizeof(SCORE_TYPE));
    assert(NULL != (*similarity_matrix)[0]);
    (*similarity_matrix)[1] = (SCORE_TYPE (*)[])calloc(4, sizeof(SCORE_TYPE));
    assert(NULL != (*similarity_matrix)[1]);
    (*similarity_matrix)[2] = (SCORE_TYPE (*)[])calloc(4, sizeof(SCORE_TYPE));
    assert(NULL != (*similarity_matrix)[2]);
    (*similarity_matrix)[3] = (SCORE_TYPE (*)[])calloc(4, sizeof(SCORE_TYPE));
    assert(NULL != (*similarity_matrix)[3]);

    return similarity_matrix;
}

void free_similarity_matrix(SCORE_TYPE (*(*SM)[])[]) {
    assert(NULL != SM);

    for (register unsigned short int i = 0; i < 4; i++) {
        free(  (*SM)[i]  );
    }
    free(SM);
}

char *create_sequence(int t) {

    size_t seq_size = 0;
    char *sequence = get_sequence(t);
    seq_size = get_sequence_size(sequence);

    char *actual_sequence = (char *)calloc(seq_size + 1, sizeof(char)); //+1 for \0
    assert(NULL != actual_sequence);

    copy_sequence(sequence, actual_sequence, seq_size);
    free(sequence);

    return actual_sequence;
}

void free_sequence(char *sequence) {
    assert(NULL != sequence);

    free(sequence);
}

SCORE_TYPE (*(*create_score_table(size_t seq1_size, size_t seq2_size))[])[] {

    SCORE_TYPE (*(*score_table)[])[] = (SCORE_TYPE (*(*)[])[])calloc(seq1_size + 1, sizeof(SCORE_TYPE (*)[]));
    assert(NULL != score_table);
    for (register size_t i = 0; i <= seq1_size; i++) {
        (*score_table)[i] = (SCORE_TYPE (*)[])calloc(seq2_size + 1, sizeof(SCORE_TYPE));
        assert(NULL != (*score_table)[i]);
    }

    return score_table;
} //(seq1_size + 1)*(seq2_size + 1)

void free_score_table(SCORE_TYPE (*(*score_table)[])[], size_t size1, size_t size2) {
    assert(NULL != score_table);

    for (register size_t i = 0; i <= size1; i++) {
        free( (*score_table)[i]  );
    }

    free(score_table);
}

SCORE_TYPE determine_similarity_score(SCORE_TYPE (*(*similarity_matrix)[])[], const char char1, const char char2) {
    assert(NULL != similarity_matrix);

    size_t SM_index1 = 0;
    if (char1 == 'A') {
        SM_index1 = A;
    } else if (char1 == 'T') {
        SM_index1 = T;
    } else if (char1 == 'G') {
        SM_index1 = G;
    } else {
        SM_index1 = C;
    }

    size_t SM_index2 = 0;
    if (char2 == 'A') {
        SM_index2 = A;
    } else if (char2 == 'T') {
        SM_index2 = T;
    } else if (char2 == 'G') {
        SM_index2 = G;
    } else {
        SM_index2 = C;
    }
    SCORE_TYPE similarity_score = SM(SM_index1, SM_index2);

    return similarity_score;
}

SCORE_TYPE BO3(SCORE_TYPE x, SCORE_TYPE y, SCORE_TYPE z) {

    if ((x > y) && (x > z)) {
        return x;
    } else if (y > z) {
        return y;
    } else {
        return z;
    }
}

SCORE_TYPE fill_score_table(SCORE_TYPE (*(*const score_table)[])[], size_t size1, size_t size2,
                            SCORE_TYPE (*(*const similarity_matrix)[])[],
                            const char *const seq1, const char *const seq2) {
    assert(NULL != score_table);
    assert(NULL != similarity_matrix);
    assert(NULL != seq1);
    assert(NULL != seq2);

    //set basis
    for (register size_t i = 0; i <= size2; i++) {
        ST(0, i) = GAP_PENALTY_INT * i;
    }
    for (register size_t j = 0; j <= size1; j++) {
        ST(j, 0) = GAP_PENALTY_INT * j;
    }
    //

    //compute score table
    for (register size_t i = 1; i <= size1; i++) {
        for (register size_t j = 1; j <= size2; j++) {
            SCORE_TYPE match = ST(i-1, j-1) +
                               determine_similarity_score(similarity_matrix, seq1[i - 1], seq2[j - 1]);
            SCORE_TYPE delete = ST(i - 1, j) + GAP_PENALTY_INT;
            SCORE_TYPE insert = ST(i, j - 1) + GAP_PENALTY_INT;
            ST(i, j) = BO3(match, delete, insert);
        }
    }
    //

    return ST(size1, size2);
}

void print_aligned_sequence(const char *const sequence, size_t size) {
    assert(NULL != sequence);

    fprintf(stdout, "\n");
    for (register size_t i = 0; i < size; i++) {
        if ('\0' == sequence[i]) {
            continue;
        } else {
            fprintf(stdout, "%c", sequence[i]);
        }
    }
}

void get_alignment(SCORE_TYPE (*(*const score_table)[])[], size_t size1, size_t size2,
                   SCORE_TYPE (*(*const similarity_matrix)[])[],
                   const char *const seq1, const char *const seq2) {
    assert(NULL != score_table);
    assert(NULL != similarity_matrix);
    assert(NULL != seq1);
    assert(NULL != seq2);
    MY_LOG("\nin {%s}", __FUNCTION__);
    char *aligned_seq1 = (char *)calloc(size1 + size2, sizeof(char));
    assert(NULL != aligned_seq1);
    aligned_seq1[size1 + size2 - 1] = '\0';
    char *aligned_seq2 = (char *)calloc(size1 + size2, sizeof(char));
    assert(NULL != aligned_seq2);
    aligned_seq2[size1 + size2 - 1] = '\0';
    size_t aligning_index = size1 + size2 - 2;
    size_t i = size1;
    size_t j = size2;
    //
    register unsigned short int decision = 0; //--------------------------------------------------------------------------------------------
    register unsigned short int bottom_gap_len = 0; //--------------------------------------------------------------------------------------------
    register unsigned short int top_gap_len = 0; //--------------------------------------------------------------------------------------------
    //
    while ((i > 0) || (j > 0)) {
        SCORE_TYPE local_score = 0;
        if ((i > 0) && (j > 0)) {
            local_score = determine_similarity_score(similarity_matrix, seq1[i - 1], seq2[j - 1]);
        }
        decision = 0;
        MY_LOG("\n---for %zu from 1st seq and %zu from 2nd seq local_score is (%d)", i, j, local_score);
        if (((i > 0) && (j > 0)) && (ST(i,j) == (ST(i - 1, j - 1) + local_score))) {
            decision = 1; //-----------------------------------------------------------------------------------------------------------------
        }
        if ((i > 0) && (ST(i,j) == (ST(i - 1, j) + GAP_PENALTY_INT))) {
            if (0 != bottom_gap_len) {
                decision = 2; //--------------------------------------------------------------------------------------------
            }
            if (0 == decision) {
                decision = 2; //--------------------------------------------------------------------------------------------
            }
        }
        if ((j > 0) && (ST(i,j) == (ST(i, j - 1) + GAP_PENALTY_INT))){
            if (0 != top_gap_len) {
                decision = 3; //----------------------------------------------------------------------------------------------------------------------
            }
            if (0 == decision) {
                decision = 3; //--------------------------------------------------------------------------------------------
            }
        }
        if (2 == decision) { //--------------------------------------------------------------------------------------------------------------
            MY_LOG("\n---(%zu) from 1st seq should be aligned with '-'", i);
            aligned_seq1[aligning_index] = seq1[i - 1];
            MY_LOG("\n------in aligned_seq1 putting {%c} in slot (%zu)", seq1[i - 1], aligning_index);
            aligned_seq2[aligning_index] = '-';
            MY_LOG("\n------in aligned_seq2 putting '-' in slot (%zu)", aligning_index);
            i -= 1;
            bottom_gap_len += 1;
            top_gap_len = 0;
        } else if (1 == decision) { //-------------------------------------------------------------------------------------------------
            MY_LOG("\n---letters (%zu) from 1st seq and (%zu) from 2nd seq should be aligned", i, j);
            aligned_seq1[aligning_index] = seq1[i - 1];
            MY_LOG("\n------in aligned_seq1 putting {%c} in slot (%zu)", seq1[i - 1], aligning_index);
            aligned_seq2[aligning_index] = seq2[j - 1];
            MY_LOG("\n------in aligned_seq2 putting {%c} in slot (%zu)", seq2[j - 1], aligning_index);
            i -= 1;
            j -= 1;
            bottom_gap_len = 0;
            top_gap_len = 0;
        } else { //------------------------------------------------------------------------------------------------------------------------
            MY_LOG("\n---%zu from 2nd seq should be aligned with '-'", j);
            aligned_seq1[aligning_index] = '-';
            MY_LOG("\n------in aligned_seq1 putting '-' in slot (%zu)", aligning_index);
            aligned_seq2[aligning_index] = seq2[j - 1];
            MY_LOG("\n------in aligned_seq2 putting {%c} in slot (%zu)", seq2[j - 1], aligning_index);
            j -= 1;
            bottom_gap_len = 0;
            top_gap_len += 1;
        }
        aligning_index -= 1;
    }

    fprintf(stdout, "\n\nalignment is...");
    print_aligned_sequence(aligned_seq1, size1 + size2 - 1);
    print_aligned_sequence(aligned_seq2, size1 + size2 - 1);
    fprintf(stdout, "\n");
    free(aligned_seq1);
    free(aligned_seq2);
}

int main() {

    //create similarity matrix
    SCORE_TYPE (*(*similarity_matrix)[])[] = create_similarity_matrix();
    //fill_similarity_matrix(similarity_matrix);
    //fill_similarity_matrix_BLOSUM62(similarity_matrix);
    fill_similarity_matrix_DNAfull(similarity_matrix);
    //

    char *sequence1 = create_sequence(1);
    size_t seq1_size = get_sequence_size(sequence1);
    char *sequence2 = create_sequence(2);
    size_t seq2_size = get_sequence_size(sequence2);

    //------------------------------------------------------------------------------------------------------------------
    print_similarity_matrix(similarity_matrix);
    print_sequence(sequence1);
    print_sequence(sequence2);
    //------------------------------------------------------------------------------------------------------------------

    SCORE_TYPE (*(*score_table)[])[] = create_score_table(seq1_size, seq2_size); //seq1_size rows, seq2_size columns
    //------------------------------------------------------------------------------------------------------------------
    print_score_table(score_table, seq1_size, seq2_size);
    //------------------------------------------------------------------------------------------------------------------

    //do the algorithm
    SCORE_TYPE aligning_score = fill_score_table(score_table, seq1_size, seq2_size,
                                                 similarity_matrix,
                                                 sequence1, sequence2);
    fprintf(stdout, "\n---aligning score is: %d\n", aligning_score);

    //------------------------------------------------------------------------------------------------------------------
    print_score_table(score_table, seq1_size, seq2_size);
    //------------------------------------------------------------------------------------------------------------------

    //alignment
    get_alignment(score_table, seq1_size, seq2_size, similarity_matrix, sequence1, sequence2);
    //
    fprintf(stdout, "\n---aligning score is: %d\n", aligning_score);
    //free memory
    free_similarity_matrix(similarity_matrix);
    free_sequence(sequence1);
    free_sequence(sequence2);
    free_score_table(score_table, seq1_size, seq2_size);
    //
    return 0;
}
