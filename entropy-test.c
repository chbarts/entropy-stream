#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <float.h>
#include "handle_ferr.h"
#include "entropy.h"

static ssize_t goslow(enctx * ctx, int fd)
{
    unsigned char buf[BUFSIZ];
    ssize_t len;

    while ((len = read(fd, buf, BUFSIZ)) > 0)
        en_add_buf(ctx, buf, (size_t) len);

    return len;
}

int main(int argc, char *argv[])
{
    int fd, i;
    unsigned char *map;
    enctx *ctx;
    struct stat st;

    if (argc == 1) {
        if ((ctx = en_start()) == NULL) {
            fprintf(stderr, "en_start() failed\n");
            return 0;
        }

        if (goslow(ctx, 0) < 0) {
            fprintf(stderr, "read() failed: ");
            handle_ferr("stdin", argv[0]);
        }

        printf("stdin\t%.*Lg\n", LDBL_DIG, en_entropy_ld(ctx));
        en_end(ctx);
        return 0;
    }

    for (i = 1; i < argc; i++) {
        if ((fd = open(argv[i], O_RDONLY)) == -1) {
            handle_ferr(argv[i], argv[0]);
            continue;
        }

        if (fstat(fd, &st) == -1) {
            fprintf(stderr, "fstat error: ");
            handle_ferr(argv[i], argv[0]);
            close(fd);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            fprintf(stderr, "%s: %s is a directory\n", argv[0], argv[i]);
            close(fd);
            continue;
        }

        if ((ctx = en_start()) == NULL) {
            fprintf(stderr, "en_start() failed\n");
            munmap(map, st.st_size);
            close(fd);
            continue;
        }

        if (S_ISREG(st.st_mode)) {
            if (st.st_size == 0) {
                fprintf(stderr, "%s: file %s zero-length\n", argv[0],
                        argv[i]);
                close(fd);
                continue;
            }

            if ((map =
                 mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd,
                      0)) == MAP_FAILED) {
                fprintf(stderr, "mmap error: ");
                handle_ferr(argv[i], argv[0]);
                close(fd);
                continue;
            }

            en_add_buf(ctx, map, st.st_size);
            printf("%s\t%.*Lg\n", argv[i], LDBL_DIG, en_entropy_ld(ctx));
            en_end(ctx);

            if (munmap(map, st.st_size) == -1) {
                fprintf(stderr, "munmap() failed: ");
                handle_ferr(argv[i], argv[0]);
            }
        } else {
            if (goslow(ctx, fd) < 0) {
                fprintf(stderr, "read() failed: ");
                handle_ferr(argv[i], argv[0]);
            }

            printf("%s\t%.*Lg\n", argv[i], LDBL_DIG, en_entropy_ld(ctx));
            en_end(ctx);
        }

        if (close(fd) == -1) {
            fprintf(stderr, "close() failed: ");
            handle_ferr(argv[i], argv[0]);
        }
    }

    return 0;
}
