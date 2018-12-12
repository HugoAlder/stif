#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "stif.h"

#define PROGNAME "spcheck"

#define FATAL(msg) fatal(__FILE__, __LINE__, msg)
static inline void fatal(const char *file, int line, const char *message)
{
	fprintf(stderr, "fatal error: (%s, %d): %s\n", file, line, message);
	exit(EXIT_FAILURE);
}

static void usage(void)
{
	fprintf(stderr, "usage: %s [options]\n", PROGNAME);
	fprintf(stderr, "\t--help,-h      : display this help\n");
	fprintf(stderr, "\t--input,-i     : stif image to parse\n");
	fprintf(stderr, "\t--dump,-d      : dump image information\n");
	fprintf(stderr, "\t--view, -v     : view stif image\n");
	exit(EXIT_FAILURE);
}

static unsigned char *read_file(const char *input_file, size_t *size)
{
	FILE *file = NULL;
	unsigned char *data;
	size_t nmemb;

	assert(input_file != NULL);
	assert(size != NULL);

	file = fopen(input_file, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "error when opening file \"%s\": %s\n", input_file, strerror(errno));
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	*size = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = malloc(*size);
	if ( data == NULL )
	{
		fclose(file);
		return NULL;
	}

	nmemb = fread(data, *size, 1, file);
	fclose(file);
	if (nmemb != 1)
	{
		fprintf(stderr, "error when reading file \"%s\": %s\n", input_file, strerror(errno));
		free(data);
		return NULL;
	}

	return data;
}

static const char *stif_color_type_to_str(uint8_t type)
{
	switch ( type )
	{
		case STIF_COLOR_TYPE_GRAYSCALE:
			return "grayscale";
		case STIF_COLOR_TYPE_RGB:
			return "rgb";
		default:
			break;
	}
	
	return "unknown";
}

static void dump_stif_image(const stif_t *s)
{
	int block_count = 0;
	stif_block_t *curr;

	assert(s != NULL);

	fprintf(stdout, "stif image:\n");
	fprintf(stdout, "- width: %d\n", s->header.width);
	fprintf(stdout, "- height: %d\n", s->header.height);
	fprintf(stdout, "- type: %s\n", stif_color_type_to_str(s->header.color_type));

	/* Counting blocks
	 */
	curr = s->block_head;
	while ( curr != NULL )
	{
		block_count++;
		curr = curr->next;
	}

	fprintf(stdout, "- block count: %d\n", block_count);

	/* Dumping blocks
	 */
	block_count = 0;
	curr = s->block_head;
	while ( curr != NULL )
	{
		unsigned char *data_ptr = curr->data;
		int i;

		assert(data_ptr != NULL);

		fprintf(stdout, "- block %d:\n", block_count);
		fprintf(stdout, "  - size: %d\n", curr->block_size);
		fprintf(stdout, "  - data: ");

		for ( i = 0; i < curr->block_size; ++i )
		{
			fprintf(stdout, "%.2x ", *data_ptr);
			data_ptr++;
		}

		fprintf(stdout, "\n");

		block_count++;
		curr = curr->next;
	}
}

static bool view_stif_image(const stif_t *s)
{
	SDL_Event event;
	SDL_Renderer *renderer;
	SDL_Window *window;
	int i, j;
	
	/* Initialize SDL
	 */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "unable to init sdl\n");
		return false;
	}

	if ( SDL_CreateWindowAndRenderer(s->header.width, s->header.height, 0, &window, &renderer) )
	{
		fprintf(stderr, "unable to create sdl window or renderer\n");
		return false;
	}

	/* White background
	 */
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
	SDL_RenderClear(renderer);

	/* Draw pixels
	 */
	for ( i = 0; i < s->header.width; ++i )
	{
		for ( j = 0; j < s->header.height; ++j )
		{
			/* Set pixel color
			 */
			if ( s->header.color_type == STIF_COLOR_TYPE_GRAYSCALE )
			{
				pixel_grayscale_t pixel;

				assert(s->grayscale_pixels != NULL);
				pixel = s->grayscale_pixels[i + j * s->header.width];

				SDL_SetRenderDrawColor(renderer, pixel, pixel, pixel, 0);
				SDL_RenderDrawPoint(renderer, i, j);
			}
			else if ( s->header.color_type == STIF_COLOR_TYPE_RGB )
			{
				pixel_rgb_t pixel;

				assert(s->rgb_pixels != NULL);
				pixel = s->rgb_pixels[i + j * s->header.width];

				SDL_SetRenderDrawColor(renderer, pixel.red, pixel.green, pixel.blue, 0);
				SDL_RenderDrawPoint(renderer, i, j);
			}
		}
	}

	/* Render image
	 */
	SDL_RenderPresent(renderer);

	/* Wait for the window to be closed
	 */
	while ( 1 )
	{
		if ( SDL_PollEvent(&event) && event.type == SDL_QUIT )
			break;
	}

	/* Close SDL and stuff
	 */
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return true;
}

static struct option long_options[] =
{
	{"help",  no_argument,       NULL, 'h' },
	{"input", required_argument, NULL, 'i' },
	{"dump",  no_argument,       NULL, 'd' },
	{"view",  no_argument,       NULL, 'v' },
	{NULL,    0,                 NULL, 0   },
};

int main(int argc, char *argv[])
{
	int c, ret = EXIT_FAILURE;
	bool dump = false;
	bool view = false;
	char *input_fn = NULL;
	unsigned char *buffer;
	size_t buffer_size;
	stif_t *image;

	/* Parse program parameters
	 */
	while ( (c = getopt_long(argc, argv, "hi:dv", long_options, NULL)) != -1 )
	{
		switch ( c )
		{
			case '?': /* FALLTHROUGH */
			case 'h':
				usage();
				break;

			case 'i':
			{
				input_fn = strdup(optarg);
				if ( input_fn == NULL )
					FATAL("unable to allocate input filename");
				break;
			}

			case 'd':
			{
				dump = true;
				break;
			}

			case 'v':
			{
				view = true;
				break;
			}
		}
	}

	/* Verify there is an input file
	 */
	if ( input_fn == NULL )
	{
		fprintf(stderr, "missing stif input file\n");
		usage();
	}

	/* Reading input file
	 */
	buffer = read_file(input_fn, &buffer_size);
	if ( buffer == NULL )
	{
		fprintf(stderr, "unable to read stif file\n");
		goto end;
	}

	/* Parse stif file
	 */
	image = parse_stif(buffer, buffer_size);
	if ( image == NULL )
	{
		fprintf(stderr, "unable to parse stif file\n");
		goto end;
	}

	if ( dump )
		dump_stif_image(image);

	if ( view )
		view_stif_image(image);

	ret = EXIT_SUCCESS;

end:
	stif_free(image);
	free(buffer);
	free(input_fn);

	return ret;
}
