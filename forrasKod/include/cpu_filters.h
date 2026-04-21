#ifndef CPU_FILTERS_H
#define CPU_FILTERS_H

double cpu_grayscale(unsigned char *data, int width, int height);
double cpu_blur(unsigned char *data, int width, int height, int strength);

#endif