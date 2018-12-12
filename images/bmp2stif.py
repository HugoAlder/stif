from PIL import Image
import argparse
import struct

if __name__ == "__main__":
    # Parsing arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('input')
    parser.add_argument('output')
    args = parser.parse_args()

    # reading image file
    img = Image.open(args.input)

    # Getting size and pixels
    pixels = list(img.getdata())
    width, height = img.size
    pixels = [pixels[i * width:(i + 1) * width] for i in range(height)]

    # Building output
    with open(args.output, 'wb') as f:

        # Magic
        f.write(b'\xFE\xCA')

        # Header block
        f.write(struct.pack('<bi', 0, 9))

        # Header values
        if img.mode == 'RGB':
            mode = 1
        else:
            mode = 0

        f.write(struct.pack('<iib', width, height, mode))

        # One line = One block
        for line in pixels:
            size = len(line)

            if img.mode == 'RGB':
                size *= 3

            # Header data
            f.write(struct.pack('<bi', 1, size))

            # Pixels
            for pix in line:
                if img.mode == 'RGB':
                    for pix_value in pix:
                        f.write(struct.pack('B', pix_value))
                else:
                    f.write(struct.pack('B', pix))



