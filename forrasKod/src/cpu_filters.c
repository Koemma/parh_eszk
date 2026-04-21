#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_filters.h"

double cpu_grayscale(unsigned char *data, int width, int height)
{
    clock_t start = clock();

    int pixels = width * height;
    for (int i = 0; i < pixels; i++)
    {
        unsigned char r = data[3 * i];
        unsigned char g = data[3 * i + 1];
        unsigned char b = data[3 * i + 2];

        unsigned char gray = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);

        data[3 * i]     = gray;
        data[3 * i + 1] = gray;
        data[3 * i + 2] = gray;
    }

    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}

double cpu_blur(unsigned char *data, int width, int height, int strength)
{
    clock_t start = clock();

    int size = 2 * strength + 1;
    int window = size * size;

    unsigned char *out = malloc(3 * width * height);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int sumR = 0, sumG = 0, sumB = 0;
            int count = 0;

            for (int dy = -strength; dy <= strength; dy++)
            {
                for (int dx = -strength; dx <= strength; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0 || ny < 0 || nx >= width || ny >= height)
                        continue;

                    int idx = 3 * (ny * width + nx);
                    sumR += data[idx];
                    sumG += data[idx + 1];
                    sumB += data[idx + 2];
                    count++;
                }
            }

            int out_idx = 3 * (y * width + x);
            out[out_idx]     = sumR / count;
            out[out_idx + 1] = sumG / count;
            out[out_idx + 2] = sumB / count;
        }
    }

    memcpy(data, out, 3 * width * height);
    free(out);

    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC;
}