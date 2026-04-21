#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "kernel_loader.h"
#include "images.h"
#include "cpu_filters.h"

void plotGPU(const char *csv_filename, const char *met, const char *image, FILE *file)
{
    FILE *fp = fopen(csv_filename, "r");
    if (!fp)
    {
        printf("Failed to open the file\n");
        return;
    }

    char line[256];
    char filename[256], method[256];
    int width, height;
    float time_val;
    double speedup;
    double cpu_time;

    fgets(line, sizeof(line), fp);

    printf("\nGPU results (%s, %s):\n", image, met);

    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\r\n")] = 0;

        if (sscanf(line, " %[^;];%d x %d;%[^;];%lf;%f;%lf",
                   filename, &width, &height, method, &cpu_time, &time_val, &speedup) == 7)
        {
            int ok = 0;

            if (image != NULL)
            {
                if (strcmp(filename, image) == 0)
                    ok = 1;
            }
            else
            {
                if (met != NULL && strcmp(method, met) == 0)
                    ok = 1;
            }

            if (ok)
            {
                int bars = (int)(time_val * 1000);
                if (bars < 1)
                    bars = 1;

                printf("%s: %dx%d, %s, %.6f | ", filename, width, height, method, time_val);

                for (int i = 0; i < bars; i++)
                    printf("#");
                printf("\n");

                if (file)
                {
                    fprintf(file, "%s;%d x %d;%s;%.6f;", filename, width, height, method, time_val);

                    for (int i = 0; i < bars; i++)
                        fprintf(file, "#");

                    fprintf(file, "\n");
                }
            }
        }
    }

    fclose(fp);
}

int main()
{
    cl_int err;
    int error_code;

    cl_uint n_platforms;
    cl_platform_id platform_id;
    err = clGetPlatformIDs(1, &platform_id, &n_platforms);
    if (err != CL_SUCCESS)
    {
        printf("[ERROR] Error calling clGetPlatformIDs. Error code: %d\n", err);
        return 0;
    }

    cl_device_id device_id;
    cl_uint n_devices;
    err = clGetDeviceIDs(
        platform_id,
        CL_DEVICE_TYPE_GPU,
        1,
        &device_id,
        &n_devices);
    if (err != CL_SUCCESS)
    {
        printf("[ERROR] Error calling clGetDeviceIDs. Error code: %d\n", err);
        return 0;
    }

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (!context || err != CL_SUCCESS)
    {
        printf("Context error!\n");
        return 0;
    }

    const char *kernel_code = load_kernel_source("kernels/feladat.cl", &error_code);
    if (error_code != 0)
    {
        printf("Source code loading error!\n");
        return 0;
    }

    cl_program program = clCreateProgramWithSource(context, 1, &kernel_code, NULL, &err);
    if (err != CL_SUCCESS)
    {
        printf("Program error!\n");
        return 0;
    }

    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Build error! Code: %d\n", err);
        size_t real_size;
        err = clGetProgramBuildInfo(
            program,
            device_id,
            CL_PROGRAM_BUILD_LOG,
            0,
            NULL,
            &real_size);
        char *build_log = (char *)malloc(sizeof(char) * (real_size + 1));
        err = clGetProgramBuildInfo(
            program,
            device_id,
            CL_PROGRAM_BUILD_LOG,
            real_size + 1,
            build_log,
            &real_size);
        printf("Real size : %d\n", real_size);
        printf("Build log : %s\n", build_log);
        free(build_log);
        return 0;
    }

    cl_kernel kernels[5];

    kernels[0] = clCreateKernel(program, "grayscale_kernel", &err);
    kernels[1] = clCreateKernel(program, "red_kernel", &err);
    kernels[2] = clCreateKernel(program, "green_kernel", &err);
    kernels[3] = clCreateKernel(program, "blue_kernel", &err);
    kernels[4] = clCreateKernel(program, "blur_kernel", &err);

    for (int i = 0; i < 5; i++)
    {
        if (kernels[i] == NULL || err != CL_SUCCESS)
        {
            printf("Kernel creation error at index: %d, err: %d\n", i, err);
            return 0;
        }
    }

    cl_queue_properties props[] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device_id, props, &err);
    if (err != CL_SUCCESS)
    {
        printf("Queue error!\n");
        return 0;
    }

    const char *images[] = {
        "images/kep01.ppm",
        "images/kep02.ppm",
        "images/kep03.ppm"};
    int image_count = sizeof(images) / sizeof(images[0]);

    int strength = 8;
    const char *modeNames[] = {"gray", "red", "green", "blue", "blur"};
    double runtime;

    FILE *f = fopen("images/results/results.csv", "w");
    fprintf(f, "Name;Width x Height;Method;CPU time (s);GPU time (s);Speedup\n");

    for (int img_idx = 0; img_idx < image_count; img_idx++)
    {
        const char *inputFile = images[img_idx];

        for (int m = 0; m < 5; m++)
        {
            Image img = readPPM(inputFile);

            size_t imgSize = 3 * img.width * img.height * sizeof(unsigned char);

            double cpu_time = 0;

            if (m == 0)
            {
                unsigned char *cpu_data = malloc(imgSize);
                memcpy(cpu_data, img.data, imgSize);

                cpu_time = cpu_grayscale(cpu_data, img.width, img.height);
                printf("CPU grayscale: %.6f s\n", cpu_time);

                free(cpu_data);
            }

            if (m == 4)
            {
                unsigned char *cpu_data = malloc(imgSize);
                memcpy(cpu_data, img.data, imgSize);

                cpu_time = cpu_blur(cpu_data, img.width, img.height, strength);
                printf("CPU blur: %.6f s\n", cpu_time);

                free(cpu_data);
            }

            cl_mem dImage = clCreateBuffer(
                context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                imgSize,
                img.data,
                &err);

            if (err != CL_SUCCESS)
            {
                printf("Buffer error!\n");
                continue;
            }

            cl_kernel kernel = kernels[m];

            clSetKernelArg(kernel, 0, sizeof(cl_mem), &dImage);
            clSetKernelArg(kernel, 1, sizeof(int), &img.width);
            clSetKernelArg(kernel, 2, sizeof(int), &img.height);

            if (m == 4)
            {
                clSetKernelArg(kernel, 3, sizeof(int), &strength);
            }

            size_t globalSize[2] = {(size_t)img.width, (size_t)img.height};

            cl_event event;
            clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, &event);
            clFinish(queue);

            cl_ulong start, end;
            clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
            clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);

            runtime = (end - start) * 1e-9;

            clEnqueueReadBuffer(
                queue,
                dImage,
                CL_TRUE,
                0,
                imgSize,
                img.data,
                0,
                NULL,
                NULL);

            double speedup;
            if(cpu_time == 0)
                speedup = 0;
            else
                speedup = cpu_time / runtime;

            printf("(GPU) %s: %d x %d, method: %s, runtime: %.6f, speedup: %.6f\n", inputFile, img.width, img.height, modeNames[m], runtime, speedup);
            fprintf(f, "%s;%d x %d;%s;%.6lf;%.6f;%.6lf\n", inputFile, img.width, img.height, modeNames[m], cpu_time, runtime, speedup);

            char outputFile[100];
            char name[50];

            const char *base = strrchr(inputFile, '/');
            if (base)
                base++;
            else
                base = inputFile;

            sscanf(base, "%[^.]", name);
            sprintf(outputFile, "images/results/%s_%s.ppm", name, modeNames[m]);

            writePPM(outputFile, img);

            clReleaseMemObject(dImage);
            clReleaseEvent(event);
            free(img.data);
        }
    }

    fclose(f);

    FILE *file = fopen("images/results/charts.csv", "w");

    printf("\n Execution time (kep01.ppm):\n");
    fprintf(file, "\nRuntime chart (kep01.ppm):\n");
    plotGPU("images/results/results.csv", NULL, "images/kep01.ppm", file);

    printf("\n Filter chart:\n");
    plotGPU("images/results/results.csv", "blur", NULL, file);

    fclose(file);

    clReleaseCommandQueue(queue);
    for (int i = 0; i < 5; i++)
        clReleaseKernel(kernels[i]);
    clReleaseProgram(program);
    clReleaseContext(context);

    return 0;
}