#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <omp.h>

#define SIZE 5

char *words[SIZE] = {"lorem", "ipsum", "dolor", "sit", "amet"};
int counts[SIZE] = {0};

void to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int get_count(const char *file_name, const char *key) {
    char lower_key[100];
    strcpy(lower_key, key);
    to_lower(lower_key);

    FILE *file = fopen(file_name, "r");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", file_name);
        return 0;
    }

    char word[100];
    int count = 0;
    while (fscanf(file, "%99s", word) == 1) {
        to_lower(word);
        if (strcmp(lower_key, word) == 0) {
            count++;
        }
    }
    fclose(file);
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    omp_set_num_threads(num_threads);

    double t = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < SIZE; i++) {
        counts[i] = get_count("words.txt", words[i]);
    }

    t = omp_get_wtime() - t;
    printf("Time: %f seconds\n", t);

    for (int i = 0; i < SIZE; i++) {
        printf("%s %d\n", words[i], counts[i]);
    }

    return 0;
}