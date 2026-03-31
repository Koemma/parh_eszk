__kernel void green_kernel(__global unsigned char* data,
                           int width,
                           int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height)
        return;

    int idx = 3 * (y * width + x);

    data[idx]     = 0;
    data[idx + 2] = 0;
}