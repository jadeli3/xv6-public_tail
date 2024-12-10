#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define BUF_SIZE 1024

void printlines(int file_descriptor, char *file_name, int lines_to_print) {
    int buffer_index = 0;
    char buffer[BUF_SIZE];
    int bytes_read = 0;
    int total_lines = 0;
    int current_line_count = 0;
    char last_char = '\0';

    int temp_file_descriptor = open("temporary_file", O_CREATE | O_RDWR);
    if (temp_file_descriptor < 0) {
        printf(1, "tail: error creating temporary file\n");
        exit();
    }

    while ((bytes_read = read(file_descriptor, buffer, sizeof(buffer))) > 0) {
        write(temp_file_descriptor, buffer, bytes_read);
        for (buffer_index = 0; buffer_index < bytes_read; buffer_index++) {
            if (buffer[buffer_index] == '\n') {
                total_lines++;
            }
            last_char = buffer[buffer_index];
        }
    }

    if (last_char != '\n') {
        total_lines++;
    }

    if (bytes_read < 0) {
        printf(1, "tail: read error\n");
        close(temp_file_descriptor);
        unlink("temporary_file");
        exit();
    }

    close(temp_file_descriptor);

    temp_file_descriptor = open("temporary_file", O_RDONLY);

    while ((bytes_read = read(temp_file_descriptor, buffer, sizeof(buffer))) > 0) {
        for (buffer_index = 0; buffer_index < bytes_read; buffer_index++) {
            if (current_line_count >= (total_lines - lines_to_print)) {
                printf(1, "%c", buffer[buffer_index]);
            }
            if (buffer[buffer_index] == '\n') {
                current_line_count++;
            }
        }
    }

    if (last_char != '\n') {
        printf(1, "\n");
    }

    close(temp_file_descriptor);
    unlink("temporary_file");
}

int main(int argc, char *argv[]) {
    int arg_index = 0;
    int file_descriptor = 0;
    int lines_to_print = 10; // default to 10 lines
    char *file_name = "";

    if (argc > 1) {
        for (arg_index = 1; arg_index < argc; arg_index++) {
            char *current_arg = argv[arg_index];

            if (current_arg[0] == '-') {
                current_arg++;
                lines_to_print = atoi(current_arg);
            } else {
                file_name = current_arg;
                file_descriptor = open(file_name, O_RDONLY);

                if (file_descriptor < 0) {
                    printf(1, "tail: cannot open file %s\n", file_name);
                    return 1;
                }
            }
        }
    }

    printlines(file_descriptor, file_name, lines_to_print);
    close(file_descriptor);
    exit();
}