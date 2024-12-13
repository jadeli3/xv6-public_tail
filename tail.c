#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define BUFFER_CAPACITY 1024

void tail_lines(int fd, char *fname, int nlines) {
    int read_count = 0, idx = 0, total_newlines = 0, current_count = 0;
    char tempbuf[BUFFER_CAPACITY];
    char last_char = 0;

    int temp_fd = open("tempfile", O_CREATE | O_RDWR);
    if (temp_fd < 0) {
        printf(1, "tail: failed to create tempfile\n");
        exit();
    }

    while ((read_count = read(fd, tempbuf, sizeof(tempbuf))) > 0) {
        if (write(temp_fd, tempbuf, read_count) != read_count) {
            printf(1, "tail: write error\n");
            close(temp_fd);
            unlink("tempfile");
            exit();
        }
        for (idx = 0; idx < read_count; idx++) {
            if (tempbuf[idx] == '\n') {
                total_newlines++;
            }
            last_char = tempbuf[idx];
        }
    }

    if (last_char != '\n') {
        total_newlines++;
    }

    if (read_count < 0) {
        printf(1, "tail: read error\n");
        close(temp_fd);
        unlink("tempfile");
        exit();
    }

    close(temp_fd);

    temp_fd = open("tempfile", O_RDONLY);
    if (temp_fd < 0) {
        printf(1, "tail: failed to reopen tempfile\n");
        unlink("tempfile");
        exit();
    }

    int startLine = total_newlines - nlines;
    if (startLine < 0) {
        startLine = 0;
    }

    while ((read_count = read(temp_fd, tempbuf, sizeof(tempbuf))) > 0) {
        for (idx = 0; idx < read_count; idx++) {
            if (current_count >= startLine) {
                printf(1, "%c", tempbuf[idx]);
            }
            if (tempbuf[idx] == '\n') {
                current_count++;
            }
        }
    }

    if (last_char != '\n') {
        printf(1, "\n");
    }

    close(temp_fd);
    unlink("tempfile");
}

int main(int argc, char *argv[]) {
    int i = 0;
    int fd = 0;
    int lines_to_show = 10; // default
    char *filename = "";

    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            char *arg = argv[i];

            if (arg[0] == '-') {
                arg++;
                lines_to_show = atoi(arg);
            } else {
                filename = arg;
                fd = open(filename, O_RDONLY);
                if (fd < 0) {
                    printf(1, "tail: cannot open file %s\n", filename);
                    return 1;
                }
            }
        }
    }

    tail_lines(fd, filename, lines_to_show);
    close(fd);
    exit();
}
