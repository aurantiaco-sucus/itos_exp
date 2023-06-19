#include <stdio.h>
#include <errno.h>

int main() {
    // Copy main.c to output.txt

    // open input file
    FILE *input_file = fopen("main.c", "r");
    if (input_file == NULL) {
        printf("Error: could not open input file.");
        return 1;
    }

    // open output file
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        printf("Error: could not open output file.");
        fclose(input_file);
        return 1;
    }

    // copy input file to output file
    char buffer[256];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), input_file)) > 0) {
        size_t bytes_written = fwrite(buffer, sizeof(char), bytes_read, output_file);
        if (bytes_written != bytes_read) {
            printf("Error: could not write the buffer to output file.");
            fclose(input_file);
            fclose(output_file);
            return 1;
        }
    }

    // close files
    fclose(input_file);
    fclose(output_file);

    // compare 2 files (main.c and output.txt)

    // open input file and output file
    char buffer_b[256];
    FILE *input_file_b = fopen("main.c", "r");
    FILE *output_file_b = fopen("output.txt", "r");

    // compare input file and output file
    while (fread(buffer, sizeof(char), sizeof(buffer), input_file_b) > 0) {
        fread(buffer_b, sizeof(char), sizeof(buffer_b), output_file_b);
        for (int i = 0; i < 256; i++) {
            if (buffer[i] != buffer_b[i]) {
                printf("Error: files do not match.");
                fclose(input_file_b);
                fclose(output_file_b);
                return 1;
            }
        }
    }

    // close files
    printf("Files match.");
    fclose(input_file_b);
    fclose(output_file_b);

    return 0;
}