#include <stdio.h>
#include "config.h"
#include <gd.h>
#include <getopt.h>
#include <stdlib.h>

void usage();

int main(int argc, char *argv[]) {

    int option_index = 0;
    static struct option long_options[] = {
            {"help", no_argument, 0, 'h'}
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

    return 0;
}

void usage() {
    fprintf(stderr, "GDCmd Version %s (libgd %s)\n", GDCMD_VERSION_STRING, GD_VERSION_STRING);
    exit(1);
}