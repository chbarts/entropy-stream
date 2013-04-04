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
