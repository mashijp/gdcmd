#include <stdio.h>
#include "config.h"
#include <gd.h>
#include <getopt.h>
#include <stdlib.h>

void usage();

enum gdcmdImageType {GIF, JPEG, PNG, UNKNOWN};

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

int main(int argc, char *argv[]) {
    option(argc, argv);
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) fprintf(stderr, "Cannot open the file: %s\n", filename);
    fclose(fp);

    return 0;
}

void usage() {
    fprintf(stderr, "GDCmd Version %s (libgd %s)\n", GDCMD_VERSION_STRING, GD_VERSION_STRING);
    fprintf(stderr, "%s\n%s\n",
    "gdidentify [filename]",
    "ex) gdidentify /tmp/fuga.gif");
    exit(1);
}