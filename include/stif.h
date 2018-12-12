#ifndef __STIF_H_
#define __STIF_H_

/* STIF stands for Simple TIIR Image Format.
 * A STIF image is composed of:
 * * a magic
 * * blocks of data
 *
 * Blocks of data are formatted as follow:
 * * 1 byte: block type (header or image data)
 * * 4 bytes: size of data
 *
 * The first block MUST be a header block, composed of:
 * * the width (4 bytes)
 * * the height (4 bytes)
 * * color type (grayscale or RGB) (1 byte)
 *
 * A grayscale pixel is composed of one byte (0-255).
 * A RGB pixel is composed of three bytes (Red, Green, Blue).
 */

#include <stdlib.h>
#include <stdint.h>

#define STIF_MAGIC_SIZE 2
#define STIF_MAGIC 0xCAFE

#define STIF_BLOCK_HEADER_SIZE 9
typedef struct stif_header_s
{
	int32_t width;
	int32_t height;
#define STIF_COLOR_TYPE_GRAYSCALE   0
#define STIF_COLOR_TYPE_RGB         1
	int8_t  color_type;
} stif_header_t;


#define STIF_BLOCK_MIN_SIZE 5

typedef struct stif_block_s
{
#define STIF_BLOCK_TYPE_HEADER 0
#define STIF_BLOCK_TYPE_DATA   1
	int8_t  block_type;
	int32_t block_size;
	uint8_t *data;

	struct stif_block_s *next;
} stif_block_t;

typedef uint8_t pixel_grayscale_t;

typedef struct pixel_rgb_s {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} pixel_rgb_t;

typedef struct stif_s
{
	stif_header_t    header;
	pixel_grayscale_t *grayscale_pixels;
	pixel_rgb_t       *rgb_pixels;
	stif_block_t      *block_head;
} stif_t;

/*!
 * \brief Free a stif structure
 *
 * \param s the stif structure
 */
void stif_free(stif_t *s);

/*!
 * \brief Read a stif block
 *
 * \param buffer the buffer
 * \param buffer_size size of the buffer
 * \param bytes_read number of bytes read
 *
 * \return the newly allocated block (to be freed with stif_block_free) or NULL on error
 */
stif_block_t *read_stif_block(const unsigned char *buffer, size_t buffer_size, size_t *bytes_read);

/*!
 * \brief Free a stif block
 *
 * \param b the block
 */
void stif_block_free(stif_block_t *b);

/*!
 * \brief Parse a STIF 
 * This function reads a whole STIF file (magic, header and image data).
 * It will store the block in the block_head list and pixels as well (according to the color type).
 *
 * \param buffer the buffer
 * \param buffer_size size of the buffer
 *
 * \return the parsed image (to be freed with stif_free) or NULL on error
 */
stif_t *parse_stif(const unsigned char *buffer, size_t buffer_size);

#endif
