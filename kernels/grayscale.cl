__kernel void grayscale_kernel(__global unsigned char* data,
                               int width,
                               int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height)
        return;

    int idx = 3 * (y * width + x);

    unsigned char r = data[idx];
    unsigned char g = data[idx + 1];
    unsigned char b = data[idx + 2];

    unsigned char gray = (unsigned char)(0.299f*r + 0.587f*g + 0.114f*b);

    data[idx] = data[idx + 1] = data[idx + 2] = gray;
}
