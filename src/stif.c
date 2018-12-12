#include <stdio.h>
#include <string.h>

#include "stif.h"

void stif_free(stif_t *s)
{
    free(s->grayscale_pixels);
    free(s->rgb_pixels);

    stif_block_t *curr_block = malloc(sizeof(stif_block_t));
    curr_block = s->block_head;
    stif_block_t *next_block = malloc(sizeof(stif_block_t));
    next_block = s->block_head->next;
    while (next_block) {
        stif_block_free(curr_block);
        curr_block = next_block;
        next_block = curr_block->next;
    }
    stif_block_free(curr_block);
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
    printf(">>>start read\n");
    printf("buffer size %ld\n", buffer_size);

    stif_block_t *b = malloc(sizeof(stif_block_t));
    if (b == NULL)
        return NULL;

    // Reading type and data size
    memcpy(b, buffer, 5);

    printf("block type %d\n", b->block_type);
    printf("block size %d\n", b->block_size);

    // Data
    uint8_t *d = malloc((b->block_size));
    if (d == NULL)
        return NULL;
    memcpy(d, buffer + 5, b->block_size);
    b->data = d;

    // Bytes read
    *bytes_read = b->block_size;
    printf("bytes read %lu\n", *bytes_read);


    // Next
    b->next = NULL;

    printf(">>>end read\n");

    return b;
}

void stif_block_free(stif_block_t *b)
{
    // Data
    free(b->data);

    // Itself
    free(b);
}

stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size)
{
    
    printf(">>>start parse\n");
    size_t i = 0;
    uint16_t magic = 0;
    size_t * bytes_read = malloc(sizeof(size_t));
    stif_header_t h;
    stif_block_t *hb = malloc(sizeof(stif_block_t));

    stif_t *s = malloc(sizeof(stif_t));

    if (s == NULL)
        return NULL;

    memcpy(&magic, buffer, STIF_MAGIC_SIZE);
    
    // Checking magic
    if (magic != STIF_MAGIC)
        return NULL;

    printf("magic %04X\n", magic);

    // Header block and header struct
    hb = read_stif_block(buffer + STIF_MAGIC_SIZE, STIF_BLOCK_HEADER_SIZE, bytes_read);
    memcpy(&h, hb->data, STIF_BLOCK_HEADER_SIZE);

    printf("width %d\n", h.width);
    printf("height %d\n", h.height);
    printf("color type %d\n", h.color_type);

    i = STIF_MAGIC_SIZE + STIF_BLOCK_HEADER_SIZE;

    // First data block
    stif_block_t *curr = malloc(sizeof(stif_block_t));
    curr = read_stif_block(buffer + i, buffer_size, bytes_read);

    // Main loop to read blocks
    /*while (i < buffer_size) {
        curr = read_stif_block(buffer + i, sizeof(buffer), bytes_read);
        printf("i is %ld\n",i);
        prev->next = curr;
        prev = curr;
        i = i + *bytes_read;
        printf("i is now %ld\n", i);
    }*/

    s->header = h;
    s->grayscale_pixels = NULL;
    s->rgb_pixels = NULL;
    s->block_head = hb;

    free(curr);

    printf(">>>end parse\n");

    return s;
}



