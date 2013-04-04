#ifndef ENTROPY_H
#define ENTROPY_H

typedef struct enctx enctx;

enctx *en_start(void);
void en_end(enctx * ctx);
int en_add_byte(enctx * ctx, unsigned char c);
int en_add_buf(enctx * ctx, unsigned char buf[], size_t n);
int en_rem_byte(enctx * ctx, unsigned char c);
int en_rem_buf(enctx * ctx, unsigned char buf[], size_t n);
double en_entropy(enctx * ctx);
long double en_entropy_ld(enctx * ctx);

#endif                          /* ENTROPY_H */
