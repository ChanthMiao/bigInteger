
#ifndef _BIG_INTEGER_H_
#define _BIG_INTEGER_H_

#include <stddef.h>
#include <limits.h>

#define FROM_CHARS 'c'
#define FROM_BYTES 'b'

typedef unsigned long seg_nums_t;
typedef unsigned long seg_t;

typedef struct bigInteger
{
    seg_nums_t total_segments;
    seg_nums_t used_segments;
    unsigned flag;
    seg_t *value;
} big_integer;

#ifdef BIG_INTEGER_ADVANCED_USER
//The following fuction are dangerous.
seg_nums_t big_integer_init_from_chars_noalloc(big_integer *const dst, const char *const src, seg_t *const buf, const seg_nums_t total_len);
seg_nums_t big_integer_init_from_chars(big_integer *const dst, const char *const src);
seg_nums_t big_integer_init_from_bytes_noalloc(big_integer *const dst, const void *const src, size_t bytes, seg_t *const buf, const seg_nums_t total_len);
seg_nums_t big_integer_init_from_bytes(big_integer *const dst, const void *const src, size_t bytes);
big_integer big_integer_copy(const big_integer *const src, seg_t *const buf, const seg_nums_t total_seg);
seg_nums_t big_integer_shrink(big_integer *const origin);
seg_nums_t big_integer_add_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_add_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_sub_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_sub_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_mul_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_mul_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_div_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_div_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_mod_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_mod_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg);
seg_nums_t big_integer_exp_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const big_integer *const mod, const seg_nums_t total_seg);
seg_nums_t big_integer_exp_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const big_integer *const mod, const seg_nums_t total_seg);
#endif

int big_integer_is_valid(const big_integer *const data);
seg_nums_t big_integer_init(big_integer *const dst, const void *const src, const char mode, size_t bytes);
int big_integer_destory(const big_integer *const data);
seg_nums_t big_integer_write(int fd, const big_integer *const data);
int big_integer_comp_u(const big_integer *const a, const big_integer *const b);
int big_integer_comp(const big_integer *const a, big_integer *const b);
big_integer big_integer_add_u(const big_integer *const a, const big_integer *const b);
big_integer big_integer_add(const big_integer *const a, const big_integer *const b);
big_integer big_integer_sub_u(const big_integer *const a, const big_integer *const b);
big_integer big_integer_sub(const big_integer *const a, const big_integer *const b);
big_integer big_integer_mul_u(const big_integer *const a, const big_integer *const b);
big_integer big_integer_mul(const big_integer *const a, const big_integer *const b);
big_integer big_integer_div_u(const big_integer *const a, const big_integer *const b);
big_integer big_integer_div(const big_integer *const a, const big_integer *const b);
big_integer big_integer_mod_u(const big_integer *const a, const big_integer *const b);
big_integer big_integer_mod(const big_integer *const a, const big_integer *const b);
big_integer big_integer_exp_u(const big_integer *const a, const big_integer *const b, const big_integer *const mod);
big_integer big_integer_exp(const big_integer *const a, const big_integer *const b, const big_integer *const mod);
seg_nums_t big_integer_increase(big_integer *const a, const big_integer *const b);

#endif