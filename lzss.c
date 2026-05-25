#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define WINDOW_SIZE 4096
#define MIN_MATCH 3
#define MAX_MATCH 18

static uint8_t* read_file(const char* path, size_t* size)
{
    FILE* f = fopen(path, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long s = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (s < 0) {
        fclose(f);
        return NULL;
    }

    uint8_t* data = malloc((size_t)s);
    if (!data) {
        fclose(f);
        return NULL;
    }

    fread(data, 1, (size_t)s, f);
    fclose(f);

    *size = (size_t)s;
    return data;
}

static int write_file(const char* path, const uint8_t* data, size_t size)
{
    FILE* f = fopen(path, "wb");
    if (!f)
        return 0;

    fwrite(data, 1, size, f);
    fclose(f);
    return 1;
}

static void find_best_match(
    const uint8_t* data,
    size_t size,
    size_t pos,
    int* best_len,
    int* best_dist)
{
    *best_len = 0;
    *best_dist = 0;

    size_t start = (pos > WINDOW_SIZE)
    ? pos - WINDOW_SIZE
    : 0;

    for (size_t i = start; i < pos; i++) {
        int len = 0;

        while (
            len < MAX_MATCH &&
            pos + len < size &&
            data[i + len] == data[pos + len])
        {
            len++;
        }

        if (len >= MIN_MATCH && len > *best_len) {
            *best_len = len;
            *best_dist = (int)(pos - i);

            if (len == MAX_MATCH)
                return;
        }
    }
}

int compress_file(const char* input_path, const char* output_path)
{
    size_t input_size;
    uint8_t* input = read_file(input_path, &input_size);

    if (!input)
        return 0;

    size_t max_output = input_size * 2 + 1024;
    uint8_t* output = malloc(max_output);

    if (!output) {
        free(input);
        return 0;
    }

    size_t ip = 0;
    size_t op = 0;

    /* header: taille originale */
    output[op++] = (uint8_t)(input_size & 0xFF);
    output[op++] = (uint8_t)((input_size >> 8) & 0xFF);
    output[op++] = (uint8_t)((input_size >> 16) & 0xFF);
    output[op++] = (uint8_t)((input_size >> 24) & 0xFF);

    while (ip < input_size) {
        size_t flag_pos = op++;
        uint8_t flags = 0;

        for (int bit = 0; bit < 8 && ip < input_size; bit++) {
            int best_len;
            int best_dist;

            find_best_match(
                input,
                input_size,
                ip,
                &best_len,
                &best_dist
            );

            if (best_len >= MIN_MATCH) {
                flags |= (1 << bit);

                uint16_t token =
                ((best_dist - 1) << 4) |
                (best_len - MIN_MATCH);

                output[op++] = token & 0xFF;
                output[op++] = token >> 8;

                ip += best_len;
            }
            else {
                output[op++] = input[ip++];
            }
        }

        output[flag_pos] = flags;
    }

    int ok = write_file(output_path, output, op);

    free(input);
    free(output);

    return ok;
}

int decompress_file(const char* input_path, const char* output_path)
{
    size_t input_size;
    uint8_t* input = read_file(input_path, &input_size);

    if (!input || input_size < 4)
        return 0;

    size_t original_size =
    ((size_t)input[0]) |
    ((size_t)input[1] << 8) |
    ((size_t)input[2] << 16) |
    ((size_t)input[3] << 24);

    uint8_t* output = malloc(original_size);
    if (!output) {
        free(input);
        return 0;
    }

    size_t ip = 4;
    size_t op = 0;

    while (ip < input_size && op < original_size) {
        uint8_t flags = input[ip++];

        for (int bit = 0; bit < 8 && op < original_size; bit++) {
            if (flags & (1 << bit)) {
                if (ip + 1 >= input_size)
                    break;

                uint16_t token =
                input[ip] |
                (input[ip + 1] << 8);
                ip += 2;

                int dist = (token >> 4) + 1;
                int len = (token & 0xF) + MIN_MATCH;

                size_t ref = op - dist;

                for (int i = 0; i < len; i++) {
                    output[op++] = output[ref + i];
                }
            }
            else {
                if (ip >= input_size)
                    break;

                output[op++] = input[ip++];
            }
        }
    }

    int ok = write_file(output_path, output, op);

    free(input);
    free(output);

    return ok;
}

int main(int argc, char** argv)
{
    if (argc < 4) {
        printf("Usage:\n");
        printf("  %s c input output\n", argv[0]);
        printf("  %s d input output\n", argv[0]);
        return 1;
    }

    if (argv[1][0] == 'c') {
        if (!compress_file(argv[2], argv[3])) {
            printf("Compression failed\n");
            return 1;
        }

        printf("Compressed successfully\n");
    }
    else {
        if (!decompress_file(argv[2], argv[3])) {
            printf("Decompression failed\n");
            return 1;
        }

        printf("Decompressed successfully\n");
    }

    return 0;
}
