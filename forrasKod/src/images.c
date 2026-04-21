#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "images.h"

Image readPPM(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        perror("Could not open file");
        exit(1);
    }

    Image img;
    char format[3];
    int max;

    fscanf(file, "%s\n%d %d\n%d\n", format, &img.width, &img.height, &max);
    if (strcmp(format, "P6") != 0 || max != 255)
    {
        printf("The file must be in P6 PPM format\n");
        exit(1);
    }

    img.data = malloc(3 * img.width * img.height);
    fread(img.data, 3, img.width * img.height, file);
    fclose(file);
    return img;
}

void writePPM(const char *filename, Image img)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Failed to write to file");
        exit(1);
    }

    fprintf(file, "P6\n%d %d\n255\n", img.width, img.height);
    fwrite(img.data, 3, img.width * img.height, file);
    fclose(file);
}