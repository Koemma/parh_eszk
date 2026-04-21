#ifndef IMAGES_H
#define IMAGES_H

typedef struct
{
    int width, height;
    unsigned char *data;
} Image;

Image readPPM(const char *filename);
void writePPM(const char *filename, Image img);

#endif