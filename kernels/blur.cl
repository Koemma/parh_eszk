__kernel void blur_kernel(__global unsigned char* data,
                          int width,
                          int height,
                          int strength)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height)
        return;

    int idx = 3 * (y * width + x);

    int size = 2 * strength + 1;
    int halfSize = size / 2;

    int r = 0, g = 0, b = 0, count = 0;

    for (int dy = -halfSize; dy <= halfSize; dy++)
    {
        for (int dx = -halfSize; dx <= halfSize; dx++)
        {
            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < width && ny >= 0 && ny < height)
            {
                int nidx = 3 * (ny * width + nx);
                r += data[nidx];
                g += data[nidx + 1];
                b += data[nidx + 2];
                count++;
            }
        }
    }

    data[idx]     = r / count;
    data[idx + 1] = g / count;
    data[idx + 2] = b / count;
}
