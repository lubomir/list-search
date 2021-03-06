#include <config.h>
#include "trie.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Trie *
load_data(FILE *fh, const char *delimiter, int with_content, int use_compress)
{
    char *line = NULL;
    size_t len = 0;
    Trie *trie = trie_new(with_content, use_compress);
    unsigned count = 0;

    while (getline(&line, &len, fh) > 0) {
        char *pch = strchr(line, '\n');
        *pch = 0;
        if (strlen(line) <= 1)
            continue;
        char *key, *val = NULL;
        if (with_content) {
            key = strtok(line, delimiter);
            val = strtok(NULL, "\n");
            if (!val)
                continue;
        } else {
            key = line;
        }
        trie_insert(trie, key, val);
        ++count;
        if (isatty(STDOUT_FILENO) && (count % 1000) == 0) {
            printf("\rInserted %u items", count);
        }
    }

    if (isatty(STDOUT_FILENO)) {
        putchar('\r');
    }
    printf("Inserted %u items\n", count);

    free(line);

    return trie;
}

static void usage(FILE *fh, const char *prog)
{
    fprintf(fh, "Usage: %s [OPTIONS...] INPUT OUTPUT\n", prog);
}

static void help(const char *prog)
{
    usage(stdout, prog);
    puts("\nAvailable options:");
    puts("  -dDELIMITER     set delimiter between key and value");
    puts("  -e              do not store data associated with keys");
    puts("  -u              do not use compression");
    puts("  -h              print this help");
    puts("");
    puts("This is list-compile from "PACKAGE" "VERSION".");
    puts("File bug reports at <"PACKAGE_URL">.");
}

int main(int argc, char *argv[])
{
    const char *delimiter = ":";
    int with_content = 1;
    int use_compress = 1;

    int opt;
    while ((opt = getopt(argc, argv, "d:euh")) != -1) {
        switch (opt) {
        case 'd':
            delimiter = optarg;
            break;
        case 'e':
            with_content = 0;
            break;
        case 'u':
            use_compress = 0;
            break;
        case 'h':
            help(argv[0]);
            return 0;
        default:
            usage(stderr, argv[0]);
            return 1;
        }
    }

    if (optind >= argc - 1) {
        fprintf(stderr, "Expected input and output file names\n");
        return 1;
    }

    FILE *infile = NULL;
    if (strcmp(argv[optind], "-") == 0) {
        infile = stdin;
    } else {
        infile = fopen(argv[optind], "r");
    }
    if (!infile) {
        perror("Failed to open input file");
        return 2;
    }

    Trie *trie = load_data(infile, delimiter, with_content, use_compress);
    fclose(infile);

    trie_serialize(trie, argv[optind + 1]);

    trie_free(trie);

    return 0;
}
