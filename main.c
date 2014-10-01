#include <stdio.h>
#include "config.h"
#include <gd.h>
#include <getopt.h>
#include <stdlib.h>
#include <Kernel/string.h>

#define EXIT_FAILURE_COMMON 1
#define EXIT_CANNOT_OPEN_FILE 2
#define FILE_HEAD_READ_SIZE 8

void usage();

enum gdcmdImageType {
    GIF, JPEG, PNG, UNKNOWN
};

char *filename;

void option(int argc, char *argv[]) {
    int option_index = 0;
    static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                usage();
                break;
            case '?':
            default:
                usage();
        }
    }

    //TODO: read stdin if a filename option is omitted
    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        usage();
    }
    filename = argv[optind];
}

int headcmp(char c1[], char c2[], int size) {
    int result = 1;
    for (int i = 0; i < size; i++) {
        if (c1[i] != c2[i]) {
            result = 0;
            break;
        }
    }
    return result;
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open the file: %s\n", filename);
        exit(EXIT_CANNOT_OPEN_FILE);
    }
    char head[FILE_HEAD_READ_SIZE];
    size_t result = fread(head, 1, FILE_HEAD_READ_SIZE, fp);
    if (result < FILE_HEAD_READ_SIZE) {
        fprintf(stderr, "The file may not be an image file");
        exit(EXIT_FAILURE_COMMON);
    }

    char gif_head[] = {'G', 'I', 'F'};
    int gif_head_size = 3;

    char png_head[] = {0x89, 0x50, 0x4e, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    int png_head_size = 8;

    if (headcmp(head, gif_head, gif_head_size)) {
        fprintf(stdout, "GIF");
    } else if (headcmp(head, png_head, png_head_size)){
        fprintf(stdout, "PNG");
    } else {
        fprintf(stderr, "Unknown image type");
        exit(EXIT_FAILURE_COMMON);
    }

    fclose(fp);

    return 0;
}

void usage() {
    fprintf(stderr, "GDCmd Version %s (libgd %s)\n", GDCMD_VERSION_STRING, GD_VERSION_STRING);
    fprintf(stderr, "%s\n%s\n",
            "gdidentify [filename]",
            "ex) gdidentify /tmp/fuga.gif");
    exit(EXIT_FAILURE_COMMON);
}