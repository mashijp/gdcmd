#include <stdio.h>
#include "config.h"
#include <gd.h>

int main(int argc, char *argv[]) {
    printf("GDCmd Version %s (libgd %s)", GDCMD_VERSION_STRING, GD_VERSION_STRING);
    puts("");
    return 0;
}
