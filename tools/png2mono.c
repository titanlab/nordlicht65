#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TW 16
#define TH 8

#define FOURCC(a,b,c,d) ( (uint32_t) (((a)<<0) | ((b)<<8) | ((c)<<16) | ((d)<<24) ) )

static uint32_t pal[256] = { 0 };
static int32_t npal;

static int32_t find_index(uint32_t col)
{
    int32_t i;

    for(i = 0; i < npal; i++) {
        if(pal[i] == col) {
            return i;
        }
    }

    return -1;
}

static int convert(char * in, char * out, int cw, int ch, int debug)
{
    FILE * f, * g;
    uint32_t * data;
	int32_t iw, ih, comp;
    int32_t x, y, ix, iy;
    uint32_t palusage[256] = { 0 };
    uint32_t npospal = 0;
    uint32_t pospals[8][16] = { 0 };

    fprintf(stdout, "converting %s to %s\n", in, out);

	f = fopen(in, "rb");
	if(!f) {
        return 1;
    }
    g = fopen(out, "wb+");
    if(!g) {
        fclose(f);
        return 2;
    }

    npal = -1;
    data = (uint32_t *)stbi_load_from_file_with_pal(
        f, &iw, &ih, &comp, STBI_rgb_alpha, pal, &npal);
	fclose(f);

	printf("png input @ 0x%p %dx%d mode=%d palette-colors=%d\n",
        data, iw, ih, comp, npal);

    int byte = 0;

    printf("%d\n", cw);

    for(x = 0; x < iw; x+=cw) {
        for(y = 0; y < ih; y+=ch) {
            uint32_t *p = data + (y * iw) + x, *o = p;

            for(iy = 0; iy < ch; iy++) {
                int byte = 0;

                for(ix = 0; ix < cw; ix++) {
                    byte |= (p[ix] == pal[0] ? 0 : 1) << (7-ix);

                    if(((ix + 1) % 8) == 0) {
                        fwrite(&byte, 1, 1, g);
                        byte = 0;
                    }
                }

                p += iw;
            }
        }
    }

    fclose(g);


    return 0;
}

int main(int argc, char * argv[])
{
    if(argc < 5) {
        fprintf(stderr, "usage: %s [in] [out] [cw] [ch]\n", argv[0]);
        return 1;
    }

    return convert(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), 0);
}
