#include <stdio.h>
#include "config.h"
#include <gd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_CANNOT_OPEN_FILE 2
#define FILE_HEAD_READ_SIZE 8

void usage();

typedef enum {
    GIF, JPEG, PNG, BMP, UNKNOWN = 99
} gdcmdImageType;

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


gdcmdImageType getImageType(FILE *fp) {
    char head[FILE_HEAD_READ_SIZE];
    size_t result = fread(head, 1, FILE_HEAD_READ_SIZE, fp);
    rewind(fp);
    if (result < FILE_HEAD_READ_SIZE) {
        return UNKNOWN;
    }

    char gif_head[] = {'G', 'I', 'F'};
    int gif_head_size = 3;

    char png_head[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    int png_head_size = 8;

    char jpeg_head[] = {0xFF, 0xD8, 0xFF};
    int jpeg_head_size = 3;

    char bmp_head[] = {'B', 'M'};
    int bmp_head_size = 2;

    if (memcmp(head, gif_head, gif_head_size) == 0) {
        return GIF;
    } else if (memcmp(head, jpeg_head, jpeg_head_size) == 0) {
        return JPEG;
    } else if (memcmp(head, png_head, png_head_size) == 0) {
        return PNG;
    } else if (memcmp(head, bmp_head, bmp_head_size) == 0) {
        return BMP;
    } else {
        return UNKNOWN;
    }
}

char* convertImageTypeToStr(gdcmdImageType t) {
    switch(t) {
        case GIF:
            return "GIF";
        case JPEG:
            return "JPEG";
        case PNG:
            return "PNG";
        case BMP:
            return "BMP";
        case UNKNOWN:
            return "UNKNOWN";
    }
}

int main(int argc, char *argv[]) {
    option(argc, argv);
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open the file: %s\n", filename);
        exit(EXIT_CANNOT_OPEN_FILE);
    }

    gdcmdImageType t = getImageType(fp);
    fprintf(stdout, "Type: %s\n", convertImageTypeToStr(t));


    gdImagePtr ptr;
    switch(t) {
        case GIF:
            ptr = gdImageCreateFromGif(fp);
            break;
        case PNG:
            ptr = gdImageCreateFromPng(fp);
            break;
        case JPEG:
            ptr = gdImageCreateFromJpeg(fp);
            break;
        case BMP:
            ptr = gdImageCreateFromBmp(fp);
            break;
        case UNKNOWN:
            fprintf(stderr, "This file may not be an image file");
            exit(EXIT_FAILURE);
    }

    fclose(fp);

    fprintf(stdout, "Size: %dx%d\n", ptr->sx, ptr->sy);

    gdImageDestroy(ptr);

    return 0;
}

void usage() {
    fprintf(stderr, "GDCmd Version %s (libgd %s)\n", GDCMD_VERSION_STRING, GD_VERSION_STRING);
    fprintf(stderr, "%s\n%s\n",
            "gdidentify [filename]",
            "ex) gdidentify /tmp/fuga.gif");
    exit(EXIT_FAILURE);
}