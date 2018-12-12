#include <stdio.h>

#include "stif.h"

void stif_free(stif_t *s)
{
    free(s.header);
    free(s->grayscale_pixels);
    free(s->rgb_pixels);

    struct stif_block_s *curr_block = s->block_head;
    struct stif_bloc_s *next_block = s->block_head->next;
    while (next_block) {
        stif_free_block(curr_block);
        *curr_block = *next_block;
        *next_block = curr_block->next;
    }
    stif_free_block(curr_block);
}

stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read)
{
    stif_block_t *b = malloc(sizeof(stif_block_t));
    if (b == NULL)
        return NULL;

    // Type + size
    memcpy(b, buffer, 5);

    // Data
    uint8_t *d = malloc((b.block_size));
    if (d == NULL)
        return NULL;
    memcpy(d, buffer + 5, b.block_size);
    b->data = d;

    // Bytes read
    bytes_read = 5 + b.block_size;

    // Next
    b->next = NULL;

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
    int truc = 0;
    int16_t magic = 0;
    size_t bytes_read = 0;
    stif_header_t *h = malloc(sizeof(stif_header_t));
    stif_block_t *hb = malloc(sizeof(stif_block_t));

    stif_t *s = malloc(sizeof(stif_t));

    if (s == NULL)
        return NULL;

    memcpy(&magic, buffer, 2);
    
    // Checking magic
    if (magic != STIF_MAGIC)
        return NULL;

    // Header block + struct
    hb = read_stif_block(buffer + 2, 9, bytes_read);
    memcpy(h, hb->data, 9);

    i = 2 + 9;

    stif_block_t *curr = malloc(sizeof(stif_block_t));
    stif_block_t *prev = malloc(sizeof(stif_block_t));
    prev = hb;

    while (block != NULL) {
        curr = read_stif_block(buffer + i, sizeof(buffer), bytes_read);
        prev.next = curr;
        prev = curr;
        i = i + bytes_read;
    }

    s.header = &h;
    // TODO s.grayscale_pixels =
    // TODO s.rgb_pixels =
    s.block_head = hb;

    return s;
}



