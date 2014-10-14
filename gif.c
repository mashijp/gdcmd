#include <string.h>
#include <DiscRecording/DiscRecording.h>
#include "gif.h"

#define GDCMD_VERBOSE 1

char* to_bool_str(int b) {
    return b ? "true" : "false";
}

void read_gif(FILE *fp) {
    rewind(fp);

    unsigned char buffer[256];

    fread(buffer, 1, 6, fp);
    if (memcmp(buffer, "GIF", 3) != 0) {
        printf("Not Gif File");
        return;
    }

    int gif87a = 0;
    int gif89a = 0;
    if (memcmp(buffer + 3, "87a", 3) == 0) {
        gif87a = 1;
    } else if (memcmp(buffer + 3, "89a", 3) == 0) {
        gif89a = 1;
    } else {
        printf("Not Gif File");
        return;
    }

    if (GDCMD_VERBOSE) {
        printf("GIF 87a: %d, 89a: %d\n", gif87a, gif89a);
    }

    int width;
    int height;
    fread(buffer, 1, 4, fp);
    width = ((int) buffer[0]) | ((int) buffer[1]) << 8;
    height = ((int) buffer[2]) | ((int) buffer[3]) << 8;

    if (GDCMD_VERBOSE) printf("Width: %d, Height: %d\n", width, height);

    fread(buffer, 1, 1, fp);
    int globalColorTableFlag = (int) buffer[0] >> 7 & 0x01;
    int colorResolution = (((int) buffer[0] >> 4) & (0x8 - 1)) + 1;

    int sortFlag = ((int) buffer[0] >> 3) & 0x01;
    int sizeOfGlobalColorTable = (((int) buffer[0]) & (0x08 - 1));
    if (GDCMD_VERBOSE) {
        printf("GlobalColorTableFlag: %d, ColorResolution: %d, SortFlag: %d, SizeOfGlobalColorTable: %d\n",
                globalColorTableFlag, colorResolution, sortFlag, sizeOfGlobalColorTable);
    }

    fread(buffer, 1, 2, fp);

    //global color table skip

    if (globalColorTableFlag) {
        int skip = (1 << (sizeOfGlobalColorTable + 1)) * 3;

        if (GDCMD_VERBOSE) {
            printf("Skip: %d bytes\n", skip);
        }
        fseek(fp, skip, SEEK_CUR);

//        for (int i = 0; i < skip; i++) {
//            fread(buffer, 1, 1, fp);
//            printf("%02X ", buffer[0]);
//            if ((i + 1) % 32 == 0) {
//                printf("\n");
//            }
//        }
    }
//    printf("\n");


    for (;;) {
        printf("\n[Position: %07X]\n", ftell(fp));
        if (fread(buffer, 1, 1, fp) < 1) {
            break;
        }
        if (buffer[0] == 0x00) {
            // Block Terminator
            if (GDCMD_VERBOSE) puts("GIF TERMINATED");
            break;
        } if (buffer[0] == 0x3B) {
            if (GDCMD_VERBOSE) puts("Trailer");
            break;
        } else if (buffer[0] == 0x21) {
            // Extension Block
            fread(buffer, 1, 1, fp);
            int extensionType = buffer[0];
            if (GDCMD_VERBOSE) printf("ExtensionBlock Type: %02X, ", extensionType);
            for (;;) {
                fread(buffer, 1, 1, fp);
                int blockSize = buffer[0];
                printf("BlockSize: %02X, ", blockSize);
                if (blockSize == 0) {break;} //Block Terminated
                fseek(fp, blockSize, SEEK_CUR);
            }
            printf("\n");

        } else if (buffer[0] == 0x2c) {
            // ImageBlock
            if (GDCMD_VERBOSE) printf("ImageBlock\n");
            fread(buffer, 1, 8, fp); //Positions
            int lx = ((int) buffer[0]) | ((int) buffer[1]) << 8;
            int ly = ((int) buffer[2]) | ((int) buffer[3]) << 8;
            int lw = ((int) buffer[4]) | ((int) buffer[5]) << 8;
            int lh = ((int) buffer[6]) | ((int) buffer[7]) << 8;
            if (GDCMD_VERBOSE) {
                printf("lx: %d, ly: %d, lw: %d, lh: %d\n", lx, ly, lw, lh);
            }

            fread(buffer, 1, 1, fp); //Bits
            int localColorTableFlag = buffer[0] & 128;
            int interlaceFlag = buffer[0] & 64;
            int sortFlag = buffer[0] & 32;
            // 2bit reserved
            int sizeOfColorTable = buffer[0] & 7;
            if (GDCMD_VERBOSE) printf("localColorTableFlag: %s, interlaceFlag: %s, sortFlag: %s, sizeofColorTable: %d\n", to_bool_str(localColorTableFlag), to_bool_str(interlaceFlag), to_bool_str(sortFlag), sizeOfColorTable);

            fread(buffer, 1, 2, fp); //Background color index and Pixel aspect ratio

            if (localColorTableFlag) {
                if (GDCMD_VERBOSE) printf("LocalColorSkipping... ");
                int skip = (1 << (sizeOfColorTable + 1)) * 3;

                if (GDCMD_VERBOSE) {
                    printf("Skip: %d bytes\n", skip);
                }
                fseek(fp, skip, SEEK_CUR);
            }
            fread(buffer, 1, 1, fp); //LZW Minimum Code Size
            for (;;) {
                fread(buffer, 1, 1, fp);
                int blockSize = buffer[0];
                if (GDCMD_VERBOSE) {
                    printf("BlockSize: %02X, ", blockSize);
                }
                if (blockSize == 0) {
                    //Block Terminated
                    if (GDCMD_VERBOSE) {
                        printf("BLOCK TERMINATED\n");
                    }
                    break;
                }
                fseek(fp, blockSize, SEEK_CUR);
            }
        } else {
            if (GDCMD_VERBOSE) printf("Unknown flag: %X\n", buffer[0]);
        }
    }

}
