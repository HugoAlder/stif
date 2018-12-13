#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "stif.h"

void stif_free(stif_t *s)
{
    if (s == NULL) {
        return;
    }
    if (s->grayscale_pixels != NULL)
        free(s->grayscale_pixels);

    if (s->rgb_pixels != NULL)
        free(s->rgb_pixels);

    if (s->block_head == NULL)
        printf("Idiot\n");

    stif_block_free(s->block_head);
    return;    
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
    printf(">>>start read\n");

    printf("buffer size %ld\n", buffer_size);

    stif_block_t *b = malloc(sizeof(stif_block_t));
    if (b == NULL)
        return NULL;

    // Data type
    memcpy(&(b->block_type), buffer, 1);
    printf("block type %d\n", (int) b->block_type);

    // Data size
    memcpy(&(b->block_size), buffer + 1, 4);
    printf("block size %d\n", (int) b->block_size);

    b->data = malloc((size_t) b->block_size);

    // Data retrieval
    memcpy(b->data, buffer + 5, b->block_size);

    // Bytes read
    *bytes_read = 5 + b->block_size;
    printf("bytes read %lu\n", *bytes_read);

    // Next block
    b->next = NULL;

    printf(">>>end read\n");

    return b;
}

void stif_block_free(stif_block_t *b)
{
    if (b == NULL || b->data == NULL) {
        return;
    }

    // Data
    free(b->data);
    b->data = NULL;

    // Freeing next block in a recursive way
    if (b->next != NULL) {
        stif_block_free(b->next);
        free(b);
    }

    return;
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size)
{
    
    printf(">>>start parse\n");

    size_t i, j = 0;
    uint16_t magic = 0;
    size_t * bytes_read = malloc(sizeof(size_t));
    stif_header_t h = {0};
    stif_block_t *hb = NULL;
    stif_t *s = malloc(sizeof(stif_t));

    if (s == NULL)
        return NULL;

    memcpy(&magic, buffer, STIF_MAGIC_SIZE);
    
    // Checking magic
    if (magic != STIF_MAGIC)
        return NULL;

    printf("magic %04X\n", magic);

    // Header block
    hb = read_stif_block(buffer + STIF_MAGIC_SIZE, STIF_BLOCK_HEADER_SIZE, bytes_read);

    // Other header struct
    memcpy(&h, &(hb->data), STIF_BLOCK_HEADER_SIZE);

    memcpy(&(h.width), hb->data, 4);
    memcpy(&(h.height), hb->data + 4, 4);
    memcpy(&(h.color_type), hb->data + 8, 1);

    printf("width %d\n", h.width);
    printf("height %d\n", h.height);
    printf("color type %d\n", h.color_type);
    size_t image_size = (size_t) h.width * h.height;
    printf("image size %zu\n", image_size);

    // Stif data
    pixel_grayscale_t *grey = NULL;
    pixel_rgb_t *color = NULL;

    if (h.color_type == STIF_COLOR_TYPE_GRAYSCALE) {
        grey = malloc(image_size * sizeof(pixel_grayscale_t));
        if (grey == NULL) {
            printf("Error: malloc grey\n");
            return NULL;
        }
    } else if (h.color_type == STIF_COLOR_TYPE_RGB) {
        color = malloc(image_size * sizeof(pixel_rgb_t));
        if (color == NULL) {
            printf("Error: malloc color\n");
            return NULL;
        }
    } else {
        printf("Error: unknown color type\n");
        return NULL;
    }

    i = STIF_MAGIC_SIZE + STIF_BLOCK_MIN_SIZE + STIF_BLOCK_HEADER_SIZE;

    // First data block
    stif_block_t *curr = NULL;
    stif_block_t *prev = hb;

    // Loop
    while (i < buffer_size) {

        curr = read_stif_block(buffer + i, buffer_size, bytes_read);

        if (curr == NULL) {
            printf("Error: curr is NULL\n");
            return NULL;
        }

        if (h.color_type == STIF_COLOR_TYPE_GRAYSCALE) {
            int k;
            printf("block\n");
            for (k = 0; k < curr->block_size; k++) {
                printf("%x", curr->data[k]);
            }
            printf("\n");
            memcpy(grey + j, &(curr->data), (size_t) curr->block_size);
            j += (size_t) curr->block_size;
        } else if (h.color_type == STIF_COLOR_TYPE_RGB) {
            memcpy(color + j, &(curr->data), (size_t) curr->block_size);
            j += (size_t) curr->block_size;
        }

        prev->next = curr;
        prev = curr;
        printf("i %ld\n", i);
        i += *bytes_read;
    }

    s->header = h;
    s->grayscale_pixels = grey;
    s->rgb_pixels = color;
    s->block_head = hb;

    free(bytes_read);

    printf(">>>end parse\n");

    return s;
}



