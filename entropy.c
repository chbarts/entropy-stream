/*

Copyright © 2013, Chris Barts.

This file is part of entropy.

entropy is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

entropy is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with entropy.  If not, see <http://www.gnu.org/licenses/>.  */

#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    size_t state[256], len;
} enctx;

enctx *en_start(void)
{
   enctx *res;

   if ((res = malloc(sizeof(enctx))) == NULL)
      return NULL;

   res->len = 0;
   memset(res->state, 0, 256 * sizeof(size_t));
   return res;
}

void en_end(enctx * ctx) 
{
   free(ctx);
}

int en_add_byte(enctx * ctx, unsigned char c) 
{
   if (!ctx)
      return -1;

   ctx->state[c]++; 
   ctx->len++; 
   return 0;
}

int en_add_buf(enctx * ctx, unsigned char buf[], size_t n) 
{
   size_t i; 

   if (!ctx)
      return -1; 

   for (i = 0; i < n; i++)
      ctx->state[buf[i]]++; 

   ctx->len += n; 
   return 0;
}

int en_rem_byte(enctx * ctx, unsigned char c) 
{
   if (!ctx)
      return -1;

   if (ctx->len == 0)
      return -2;

   ctx->state[c]--;
   ctx->len--;
   return 0;
}

int en_rem_buf(enctx * ctx, unsigned char buf[], size_t n) 
{
   size_t i;

   if (!ctx)
      return -1;

   if (ctx->len < n)
      return -2;

   for (i = 0; i < n; i++)
      ctx->state[buf[i]]--;

   ctx->len -= n;
   return 0;
}

double en_entropy(enctx * ctx)
{
   double probs[256], acc = 0; 
   int i;

   if (!ctx)
      return 0;

   if (ctx->len == 0)
      return 0;

   for (i = 0; i < 256; i++)
      probs[i] = ((double) ctx->state[i]) / ((double) ctx->len);

   for (i = 0; i < 256; i++)
      if (probs[i] != 0)
         acc += probs[i] * log2(probs[i]);

   return -acc;
}

long double en_entropy_ld(enctx * ctx)
{
   long double probs[256], acc = 0;
   int i;

   if (!ctx)
      return 0;

   if (ctx->len == 0)
      return 0;

   for (i = 0; i < 256; i++)
      probs[i] = ((long double) ctx->state[i]) / ((long double) ctx->len);

   for (i = 0; i < 256; i++)
      if (probs[i] != 0)
         acc += probs[i] * log2l(probs[i]);

   return -acc;
}
