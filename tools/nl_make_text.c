#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

static char* tbl = "0123456789:!\"+,?@abcdefghijklmnopqrstuvwxyz._#><()*- ";
static int trans[256];

static int convert(char * in, char * out, int debug)
{
    FILE * f, * g;
    int i, j, len = 0;
    const char * prefix = "    ";
    char str[1024];

    fprintf(stdout, "converting %s to %s\n", in, out);

	f = fopen(in, "rt");
	if(!f) {
        return 1;
    }
    g = fopen(out, "wt");
    if(!g) {
        fclose(f);
        return 2;
    }

    for(i = 0; i < 256; i++) {
        trans[i] = -1;
    }

    for(i = 0; i < strlen(tbl); i++) {
        trans[tbl[i]] = i;
    }

    while(fgets(str, sizeof(str), f)) {
        if(!strlen(str)) {
            continue;
        }

        fprintf(g, "%sbyte ", prefix);
        if(!strcmp(str, "~\n")) {
            for(i = 0; i < 8; i++) {
                fprintf(g, "$%02x", trans[' ']);
                len++;

                if(i < 8 - 1) {
                    fprintf(g, ", ");
                }
            }
        } else {
            for(i = 0; i < strlen(str); i++) {
                char c = str[i];
                if(trans[c] < 0) c = ' ';
                fprintf(g, "$%02x", trans[c]);
                len++;

                if(i < strlen(str) - 1) {
                    fprintf(g, ", ");
                }
            }
        }
        fprintf(g, "\n");
    }

    int block_len = ((int) pow(2, ceil(log(len)/log(2))) - 1);

    printf("len = %d\n", len);
    printf("block_len = %d\n", block_len);

    if(len != block_len) {
        fprintf(g, "%sbyte ", prefix);
        for(i = len; i < block_len; i++) {
            fprintf(g, "$%02x", trans[' ']);
            if(i < block_len - 1) {
                fprintf(g, ", ");
            }
        }
        fprintf(g, "\n");
    }

    fprintf(g, "%sbyte $ff\n", prefix);
    fprintf(g, "scr_text_len\n%sword $%04x\n", prefix, block_len);

    fclose(f);
    fclose(g);

    return 0;
}

int main(int argc, char * argv[])
{
    if(argc < 3) {
        fprintf(stderr, "usage: %s [in] [out]\n", argv[0]);
        return 1;
    }

    return convert(argv[1], argv[2], 0);
}
