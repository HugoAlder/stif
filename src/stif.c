#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "stif.h"

#define STIF_HEAD_WIDTH_SIZE        4
#define STIF_HEAD_HEIGHT_SIZE       4
#define STIF_HEAD_COLOR_TYPE_SIZE   1

void stif_free(stif_t *s)
{
    if (s == NULL) {
        return;
    }
    if (s->grayscale_pixels != NULL)
        free(s->grayscale_pixels);

    if (s->rgb_pixels != NULL)
        free(s->rgb_pixels);

    if (s->block_head != NULL)
        stif_block_free(s->block_head);

    free(s);

    return;    
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
    // Checking if buffer is null
    if (buffer == NULL)
        return NULL;

    // In case block buffer is smaller than the minimal possible block
    if (buffer_size < STIF_BLOCK_MIN_SIZE)
        return NULL;

    stif_block_t *b = malloc(sizeof(stif_block_t));
    if (b == NULL) {
        stif_block_free(b);
        return NULL;
    }

    // Data type
    memcpy(&(b->block_type), buffer, 1);

    // Data size
    memcpy(&(b->block_size), buffer + 1, 4);

    b->data = malloc((size_t) b->block_size);

    // Data retrieval
    memcpy(b->data, buffer + STIF_BLOCK_MIN_SIZE, b->block_size);

    // Bytes read
    *bytes_read = STIF_BLOCK_MIN_SIZE + b->block_size;

    // Next block
    b->next = NULL;

    return b;
}

void stif_block_free(stif_block_t *b)
{
    if (b == NULL || b->data == NULL) {
        return;
    }

    // Data
    free(b->data);
    if (b->next != NULL)
        stif_block_free(b->next);
    free(b);

    return;
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size)
{
    // Checking if buffer is null
    if (buffer == NULL)
        return NULL;

    size_t i, j = 0;
    uint16_t magic = 0;
    size_t bytes_read = 0;
    stif_header_t h = {0};
    stif_block_t *hb = NULL;

    stif_t *s = malloc(sizeof(stif_t));
    if (s == NULL)
        goto error;

    memcpy(&magic, buffer, STIF_MAGIC_SIZE);
    
    // Checking magic
    if (magic != STIF_MAGIC)
        goto error;

    // Header block
    hb = read_stif_block(buffer + STIF_MAGIC_SIZE, STIF_BLOCK_HEADER_SIZE, &(bytes_read));

    // Other header struct
    memcpy(&h, &(hb->data), STIF_BLOCK_HEADER_SIZE);

    memcpy(&(h.width), hb->data, STIF_HEAD_WIDTH_SIZE);
    memcpy(&(h.height), hb->data + STIF_HEAD_WIDTH_SIZE, STIF_HEAD_HEIGHT_SIZE);
    memcpy(&(h.color_type), hb->data + STIF_HEAD_WIDTH_SIZE + STIF_HEAD_HEIGHT_SIZE, STIF_HEAD_COLOR_TYPE_SIZE);

    // Invalid size
    if (h.width < 0 || h.height < 0)
        goto error;

    size_t image_size = (size_t) h.width * h.height;

    // Stif data
    pixel_grayscale_t *grey = NULL;
    pixel_rgb_t *color = NULL;

    // Allocating required space for data given the color type
    if (h.color_type == STIF_COLOR_TYPE_GRAYSCALE) {

        // Grayscale image too big
        if (image_size > buffer_size - STIF_MAGIC_SIZE - STIF_BLOCK_MIN_SIZE - STIF_BLOCK_HEADER_SIZE )
            goto error;

        grey = malloc(image_size * sizeof(pixel_grayscale_t));

        if (grey == NULL) {
            goto error;
        }
    } else if (h.color_type == STIF_COLOR_TYPE_RGB) {

        // Rgb image too big
        if (image_size > (buffer_size - STIF_MAGIC_SIZE - STIF_BLOCK_MIN_SIZE - STIF_BLOCK_HEADER_SIZE / 3))
            goto error;

        color = malloc(image_size * sizeof(pixel_rgb_t));

        if (color == NULL) {
            goto error;
        }
    } else {
        goto error;
    }

    i = STIF_MAGIC_SIZE + STIF_BLOCK_MIN_SIZE + STIF_BLOCK_HEADER_SIZE;

    // First data block
    stif_block_t *curr = NULL;
    stif_block_t *prev = hb;

    // Main loop
    while (i < buffer_size) {

        curr = read_stif_block(buffer + i, buffer_size, &(bytes_read));

        if (curr == NULL) {
            goto error;
        }

        if (h.color_type == STIF_COLOR_TYPE_GRAYSCALE) {
            memcpy(grey + j, curr->data, (size_t) curr->block_size);
        } else if (h.color_type == STIF_COLOR_TYPE_RGB) {
            memcpy(color + j / 3, curr->data, (size_t) curr->block_size);
        }

        j += (size_t) curr->block_size;
        prev->next = curr;
        prev = curr;
        printf("i %ld\n", i);
        i += bytes_read;
    }

    s->header = h;
    s->grayscale_pixels = grey;
    s->rgb_pixels = color;

    s->block_head = hb; 

    return s;

error:
    stif_free(s);
    return NULL;
}



