#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#define _GNU_SOURCE
#include <getopt.h>

#define LINES 1
#define BYTES 2

// prototype declaration
static void myhead(FILE *fp, long val, int flag);

static struct option longopts[] = {
    {"lines", required_argument, NULL, 'n'},
    {"bytes", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};


// main function
int main(int argc, char *argv[]) {
    int opt;
    long val;
    int flag, on = 0;

    while ((opt = getopt_long(argc, argv, "n:c:h", longopts, NULL)) != -1) {
        switch (opt) {
            case 'n':
                if (on == 1) {
                    fprintf(stderr, "%s: can't combine line and byte counts\n", argv[0]);
                    exit(EX_USAGE);
                }
                val = atol(optarg); // line count
                flag = LINES;
                on = 1;
                break;
            case 'c':
                if (on == 1) {
                    fprintf(stderr, "%s: can't combine line and byte counts\n", argv[0]);
                    exit(EX_USAGE);
                }
                val = atol(optarg); // byte count
                flag = BYTES;
                on = 1;
                break;
            case 'h':
                fprintf(stderr, "[USAGE]\n$ %s [-n lines | -c bytes] [file ...]\n", argv[0]);
                fprintf(stderr, "\n[DESCRIPTION]\nThis filter displays the first count lines or bytes of each of the specified files, or of the standard input if no files are specified. If count is omitted, an error occurs. (Note that this differs from the specification with the actual linux command.\n");
                fprintf(stderr, "\n[EXAMPLE]\n(1) $ %s -n 10 sample.txt\n", argv[0]);
                fprintf(stderr, "\t-> Display the first 10 lines of the file sample.txt\n");
                fprintf(stderr,              "(2) $ %s -c 10 sample.txt\n", argv[0]);
                fprintf(stderr, "\t-> Display the first 10 bytes of the file sample.txt\n");

                exit(EX_USAGE);
            case '?':
                fprintf(stderr, "[USAGE] $ %s [-n lines | -c bytes] [file ...]\n", argv[0]);
                fprintf(stderr, "[DETAIL] $ %s --help\n", argv[0]);
                exit(EX_USAGE);
        }
    }

    if (optind == argc) {       // no file are given -> read from stdin
        myhead(stdin, val, flag);
    } else {                    // file are given    -> read from the file
        int i;

        for (i = optind; i < argc; i++) {
            FILE *fp;

            if ((fp = fopen(argv[i], "r")) == NULL) {
                perror(argv[i]);
                exit(EX_NOINPUT);
            }
            myhead(fp, val, flag);

            fclose(fp);
        }
    }
    return EX_OK;
}

static void myhead(FILE *fp, long val, int flag) {
    int c;
    while ((c = getc(fp)) != EOF) {
        if (putchar(c) < 0) exit(EX_IOERR);
        if (flag == LINES) {
            if (c == '\n') {
                val --;
                if (val == 0) return;
            }
        } else if (flag == BYTES) {
            for (int i = 0; i < sizeof(char); i++) {
                val --;
            }
            if (val == 0) return;
        }
    }
}
