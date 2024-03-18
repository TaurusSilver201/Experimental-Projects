#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

int count_lines(FILE *file);
int count_words(FILE *file);
long count_characters(FILE *file);

int main() {
    FILE *file;
    char *file_name = "input.txt"; // Replace with the name of your text file
    long file_size;
    int num_lines, num_words;

    // Open the file in read mode
    file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", file_name);
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END); // Move the file pointer to the end
    file_size = ftell(file); // Get the current position (size of the file)
    rewind(file); // Move the file pointer back to the beginning

    // Count lines, words, and characters
    num_lines = count_lines(file);
    rewind(file); // Move the file pointer back to the beginning
    num_words = count_words(file);
    rewind(file); // Move the file pointer back to the beginning

    printf("File: %s\n", file_name);
    printf("Size: %ld bytes\n", file_size);
    printf("Lines: %d\n", num_lines);
    printf("Words: %d\n", num_words);
    printf("Characters: %ld\n", count_characters(file));

    // Close the file
    fclose(file);

    return 0;
}

int count_lines(FILE *file) {
    int count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        count++;
    }

    return count;
}

int count_words(FILE *file) {
    int count = 0;
    char line[MAX_LINE_LENGTH];
    char *token;

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        token = strtok(line, " \t\n");
        while (token != NULL) {
            count++;
            token = strtok(NULL, " \t\n");
        }
    }

    return count;
}

long count_characters(FILE *file) {
    long count = 0;
    int c;

    while ((c = fgetc(file)) != EOF) {
        count++;
    }

    return count;
}
