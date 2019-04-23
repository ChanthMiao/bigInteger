#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bigInteger.h"

const seg_nums_t seg_nums_zero = 0ul;
const seg_t seg_t_zero = 0ul;

static seg_t shared_buf_1[STATIC_BUF_SEG_LEN];
static seg_t shared_buf_2[STATIC_BUF_SEG_LEN];
static seg_t shared_buf_3[STATIC_BUF_SEG_LEN];

seg_nums_t big_integer_init_from_chars_noalloc(big_integer *const dst, const char *const src, seg_t *const buf, const seg_nums_t total_len)
{
    static char swap_buf[STR_SEG_WIDTH + 1ul];
    memset(swap_buf, 0, STR_SEG_WIDTH + 1ul);
    (dst->total_segments) = total_len;
    (dst->value) = buf;

    big_integer rt;
    const char *hex_str_ptr = src;
    if (*hex_str_ptr == '-')
    {
        (dst->flag) = NEGATIVE;
        hex_str_ptr++;
    }
    else if (*hex_str_ptr == '+')
    {
        (dst->flag) = POSITIVE;
        hex_str_ptr++;
    }
    else
    {
        (dst->flag) = POSITIVE;
    }
    seg_nums_t str_len = strlen(hex_str_ptr);
    seg_nums_t head_str_len = str_len % STR_SEG_WIDTH;
    (dst->used_segments) = (str_len / STR_SEG_WIDTH) + ((head_str_len) ? 1ul : 0ul);
    *(dst->value) = seg_t_zero;
    seg_t *value_ptr = (dst->value) + (dst->used_segments) - 1ul;
    char *invalid_char = NULL;
    /*Process the header.*/
    if (head_str_len)
    {
        strncpy(swap_buf, hex_str_ptr, head_str_len);
        *value_ptr = strtoul(swap_buf, &invalid_char, 16);
        value_ptr--;
        hex_str_ptr += head_str_len;
    }

    while (*hex_str_ptr != '\0')
    {
        strncpy(swap_buf, hex_str_ptr, STR_SEG_WIDTH);
        *value_ptr = strtoul(swap_buf, &invalid_char, 16);
        value_ptr--;
        hex_str_ptr += (STR_SEG_WIDTH);
    }

    while ((dst->value)[(dst->used_segments) - 1ul] == 0ul && (dst->used_segments) > 1ul)
    {
        (dst->used_segments)--;
    }
    return (dst->used_segments);
}

seg_nums_t big_integer_init_from_chars(big_integer *const dst, const char *const src)
{
    (dst->total_segments) = strlen(src) / STR_SEG_WIDTH + 1ul;
    (dst->value) = (seg_t *)calloc(dst->total_segments, SEG_MEM_WIDTH);
    return big_integer_init_from_chars_noalloc(dst, src, dst->value, (dst->total_segments));
}

seg_nums_t big_integer_init_from_bytes_noalloc(big_integer *const dst, const void *const src, size_t bytes, seg_t *const buf, const seg_nums_t total_len)
{
    (dst->flag) = POSITIVE;
    (dst->value) = buf;
    (dst->total_segments) = total_len;
    (dst->used_segments) = (bytes / SEGMENT_WIDTH) + ((bytes % SEGMENT_WIDTH) ? 1ul : seg_nums_zero);
    *(dst->value) = seg_t_zero;
    seg_nums_t cycle = (bytes / SEGMENT_WIDTH);
    seg_t *byte_ptr = (seg_t *)src;
    for (seg_nums_t i = 0; i < cycle; i++)
    {
        *((dst->value) + i) = *(byte_ptr + i);
    }
    size_t bytes_left = bytes % SEGMENT_WIDTH;
    if (bytes_left)
    {
        seg_t mask = (~seg_t_zero) >> SEG_BITS_WIDTH;
        for (size_t i = 0; i + bytes_left < SEGMENT_WIDTH; i++)
        {
            mask >>= 8u;
        }
        *((dst->value) + cycle) = (*(byte_ptr + cycle) & mask);
    }
    while ((dst->value)[(dst->used_segments) - 1ul] == 0ul && (dst->used_segments) > 1ul)
    {
        (dst->used_segments)--;
    }
    return (dst->used_segments);
}

seg_nums_t big_integer_init_from_bytes(big_integer *const dst, const void *const src, size_t bytes)
{
    (dst->total_segments) = (bytes / SEGMENT_WIDTH) + ((bytes % SEGMENT_WIDTH) ? 1ul : seg_nums_zero);
    (dst->value) = (seg_t *)calloc((dst->total_segments), SEG_MEM_WIDTH);
    return big_integer_init_from_bytes_noalloc(dst, src, bytes, dst->value, (dst->total_segments));
}

int big_integer_is_valid(const big_integer *const data)
{
    if ((data->total_segments) == 0 || (data->total_segments) < data->used_segments)
    {
        return -1;
    }
    else if ((data->value)[(data->used_segments) - 1ul] == seg_t_zero && (data->used_segments) != 1ul)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

big_integer big_integer_copy(const big_integer *const src, seg_t *const buf, const seg_nums_t total_seg)
{
    big_integer rt;
    rt.flag = (src->flag);
    rt.used_segments = (src->used_segments);
    if (buf == NULL)
    {
        rt.value = (seg_t *)calloc(rt.used_segments, SEG_MEM_WIDTH);
        rt.total_segments = rt.used_segments;
    }
    else
    {
        rt.value = buf;
        rt.total_segments = total_seg;
    }
    seg_t *dst_value_ptr = (rt.value);
    seg_t *src_value_ptr = (src->value);
    for (size_t i = 0; i < (rt.used_segments); i++)
    {
        *(dst_value_ptr + i) = *(src_value_ptr + i);
    }
    return rt;
}

seg_nums_t big_integer_shrink(big_integer *const origin)
{
    while ((origin->value)[(origin->used_segments) - 1ul] == 0ul && (origin->used_segments) > 1ul)
    {
        (origin->used_segments)--;
    }
    if ((origin->total_segments) - (origin->used_segments) > 0xfu)
    {
        (origin->value) = (seg_t *)realloc(origin->value, ((origin->used_segments) + 2u) * SEG_MEM_WIDTH);
        (origin->total_segments) = (origin->used_segments) + 2u;
    }
    return (origin->total_segments);
}

int big_integer_comp_u(const big_integer *const a, const big_integer *const b)
{
    seg_t *value_a = (a->value) + (a->used_segments) - 1ul;
    seg_t *value_b = (b->value) + (b->used_segments) - 1ul;
    if ((a->used_segments) == (b->used_segments))
    {
        while (value_a > (a->value) && *value_a == *value_b)
        {
            value_a--;
            value_b--;
        }
        return ((*value_a) < (*value_b)) ? -1 : (((*value_a) == (*value_b)) ? 0 : 1);
    }
    else
    {
        return ((a->used_segments) < (b->used_segments)) ? -1 : 1;
    }
}

seg_nums_t big_integer_add_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    big_integer x, y;
    (dst->flag) = POSITIVE;
    (dst->total_segments) = total_seg;
    if ((a->used_segments) < (b->used_segments))
    {
        x = *b;
        y = *a;
    }
    else
    {
        x = *a;
        y = *b;
    }
    seg_t *seg_ptr_x = x.value;
    seg_t *seg_ptr_y = y.value;
    seg_t *border = y.value + y.used_segments;
    seg_t *seg_ptr_dst = (dst->value);
    seg_t carry = 0ul;
    while (seg_ptr_y < border)
    {
        (*seg_ptr_dst) = (*seg_ptr_x) + (*seg_ptr_y) + carry;
        carry = (*seg_ptr_dst) / SEGMENT_MOD;
        (*seg_ptr_dst) %= SEGMENT_MOD;
        seg_ptr_dst++;
        seg_ptr_x++;
        seg_ptr_y++;
    }
    for (border = x.value + x.used_segments; seg_ptr_x < border; seg_ptr_dst++, seg_ptr_x++)
    {
        (*seg_ptr_dst) = (*seg_ptr_x) + carry;
        carry = (*seg_ptr_dst) / SEGMENT_MOD;
        (*seg_ptr_dst) %= SEGMENT_MOD;
    }
    (*seg_ptr_dst) = carry;
    (dst->used_segments) = (carry) ? x.used_segments + 1ul : x.used_segments;
    return (dst->used_segments);
}

seg_nums_t big_integer_add_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    seg_nums_t rt;
    if ((a->flag) == (b->flag))
    {
        rt = big_integer_add_u_noalloc(dst, a, b, total_seg);
        (dst->flag) = (a->flag);
    }
    else if (b->flag)
    {
        rt = big_integer_sub_u_noalloc(dst, a, b, total_seg);
    }
    else
    {
        rt = big_integer_sub_u_noalloc(dst, b, a, total_seg);
    }
    if (rt == 1ul && *(dst->value) == seg_t_zero)
    {
        (dst->flag) = POSITIVE;
    }

    return rt;
}

big_integer big_integer_add_u(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    rt.total_segments = ((a->used_segments) < (b->used_segments)) ? (b->used_segments) + 1ul : (a->used_segments) + 1ul;
    rt.value = (seg_t *)calloc(rt.total_segments, SEG_MEM_WIDTH);
    big_integer_add_u_noalloc(&rt, a, b, rt.total_segments);
    return rt;
}

seg_nums_t big_integer_sub_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    big_integer x, y;
    int comp_rt = big_integer_comp_u(a, b);
    if (comp_rt == 0)
    {
        (dst->flag) = POSITIVE;
        (dst->total_segments) = total_seg;
        (dst->used_segments) = 1ul;
        *(dst->value) = seg_t_zero;
        return (dst->used_segments);
    }
    else if (comp_rt == -1)
    {
        (dst->flag) = NEGATIVE;
        x = *b;
        y = *a;
    }
    else
    {
        (dst->flag) = POSITIVE;
        x = *a;
        y = *b;
    }
    (dst->total_segments) = total_seg;
    (dst->used_segments) = x.used_segments;
    seg_t *seg_ptr_x = x.value;
    seg_t *seg_ptr_y = y.value;
    seg_t *border = y.value + y.used_segments;
    seg_t *seg_ptr_dst = (dst->value);
    seg_t carry = 0ul;
    while (seg_ptr_y < border)
    {
        (*seg_ptr_dst) = (*seg_ptr_x) - (*seg_ptr_y) - carry;
        if ((*seg_ptr_dst) > SEGMENT_MOD)
        {
            carry = 1ul;
            (*seg_ptr_dst) += SEGMENT_MOD;
        }
        else
        {
            carry = 0ul;
        }
        seg_ptr_dst++;
        seg_ptr_x++;
        seg_ptr_y++;
    }

    for (border = x.value + x.used_segments; seg_ptr_x < border; seg_ptr_dst++, seg_ptr_x++)
    {
        (*seg_ptr_dst) = (*seg_ptr_x) - carry;
        if ((*seg_ptr_dst) > SEGMENT_MOD)
        {
            carry = 1ul;
            (*seg_ptr_dst) += SEGMENT_MOD;
        }
        else
        {
            carry = 0ul;
        }
    }
    for (; (dst->value)[(dst->used_segments) - 1ul] == 0ul && (dst->used_segments) > 1ul; (dst->used_segments)--)
    {
        continue;
    }
    return (dst->used_segments);
}

seg_nums_t big_integer_sub_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    seg_nums_t rt;
    if ((a->flag) != (b->flag))
    {
        rt = big_integer_add_u_noalloc(dst, a, b, total_seg);
        (dst->flag) = (a->flag);
    }
    else if (a->flag)
    {
        rt = big_integer_sub_u_noalloc(dst, b, a, total_seg);
    }
    else
    {
        rt = big_integer_sub_u_noalloc(dst, a, b, total_seg);
    }
    if (rt == 1ul && *(dst->value) == seg_t_zero)
    {
        (dst->flag) = POSITIVE;
    }
    return rt;
}

/*Warning: this function deals with params as unsigned big_integer, without valid check.*/
big_integer big_integer_sub_u(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    rt.total_segments = ((a->used_segments) < (b->used_segments)) ? (b->used_segments) : (a->used_segments);
    rt.value = (seg_t *)calloc(rt.total_segments, SEG_MEM_WIDTH);
    big_integer_sub_u_noalloc(&rt, a, b, rt.total_segments);
    big_integer_shrink(&rt);
    return rt;
}

seg_nums_t big_integer_mul_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    big_integer x, y;
    (dst->flag) = POSITIVE;
    (dst->total_segments) = total_seg;
    if (((a->used_segments == 1ul) && *(a->value) == seg_t_zero) || ((b->used_segments == 1ul) && *(b->value) == seg_t_zero))
    {
        (dst->used_segments) = 1ul;
        *(dst->value) = seg_t_zero;
        return (dst->used_segments);
    }
    if ((a->used_segments) < (b->used_segments))
    {
        x = *b;
        y = *a;
    }
    else
    {
        x = *a;
        y = *b;
    }
    (dst->used_segments) = (a->used_segments) + (b->used_segments);
    memset(dst->value, 0, (dst->used_segments) * SEG_MEM_WIDTH);
    seg_t tmp;
    seg_t *seg_ptr_x = x.value;
    seg_t *seg_ptr_y = y.value;
    seg_t *border_y = y.value + y.used_segments;
    seg_t *border_x = x.value + x.used_segments;
    seg_t *seg_ptr_rt = (dst->value);
    seg_t carry = 0ul;
    seg_t outer_carry = 0ul;
    for (; seg_ptr_y < border_y; seg_ptr_y++)
    {
        seg_ptr_rt = (dst->value) + (seg_ptr_y - y.value);
        seg_ptr_x = x.value;
        for (; seg_ptr_x < border_x; seg_ptr_x++)
        {
            tmp = (*seg_ptr_x) * (*seg_ptr_y) + carry;
            tmp += (*seg_ptr_rt);
            carry = tmp / SEGMENT_MOD;
            *seg_ptr_rt = tmp % SEGMENT_MOD;
            seg_ptr_rt++;
        }
        *seg_ptr_rt = (carry) ? carry : seg_t_zero;
    }

    while ((dst->value)[(dst->used_segments - 1ul)] == seg_t_zero && (dst->used_segments) > 1ul)
    {
        (dst->used_segments)--;
    }
    return (dst->used_segments);
}

seg_nums_t big_integer_mul_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    seg_nums_t rt;
    rt = big_integer_mul_u_noalloc(dst, a, b, total_seg);
    if ((dst->used_segments) == 1ul && *(dst->value) == seg_t_zero)
    {
        (dst->value) = POSITIVE;
    }
    else
    {
        (dst->flag) = ((a->flag) == (b->flag)) ? POSITIVE : NEGATIVE;
    }
    return rt;
}

big_integer big_integer_mul_u(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    rt.total_segments = (a->used_segments) + (b->used_segments);
    rt.value = (seg_t *)calloc(rt.total_segments, SEG_MEM_WIDTH);
    big_integer_mul_u_noalloc(&rt, a, b, rt.total_segments);
    big_integer_shrink(&rt);
    return rt;
}

seg_nums_t big_integer_div_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    if ((b->used_segments) == 1ul && *(b->value) == seg_t_zero)
    {
        (dst->used_segments) = 0;
        return (dst->used_segments);
    }
    (dst->flag) = POSITIVE;
    int comp_u_rt = big_integer_comp_u(a, b);
    if (comp_u_rt < 0)
    {
        (dst->used_segments) = 1ul;
    }
    else if (comp_u_rt == 0)
    {
        (dst->used_segments) = 1ul;
        *(dst->value) = 1ul;
    }
    else
    {
        (dst->used_segments) = 1ul;
        (dst->total_segments) = total_seg;
        big_integer copy_a;
        big_integer buf = *a;
        big_integer guess_this_cycle = *a;
        if ((a->used_segments) < STATIC_BUF_SEG_LEN)
        {
            copy_a = big_integer_copy(a, shared_buf_1, STATIC_BUF_SEG_LEN);
            buf.value = shared_buf_2;
            buf.total_segments = STATIC_BUF_SEG_LEN;
            guess_this_cycle.value = shared_buf_3;
            guess_this_cycle.total_segments = STATIC_BUF_SEG_LEN;
        }
        else
        {
            copy_a = big_integer_copy(a, NULL, 0);
            buf.value = (seg_t *)calloc(buf.total_segments, SEG_MEM_WIDTH);
            guess_this_cycle.value = (seg_t *)calloc(guess_this_cycle.total_segments, SEG_MEM_WIDTH);
        }
        buf.used_segments = 1ul;
        *(buf.value) = seg_t_zero;
        guess_this_cycle.used_segments = 1ul;
        *(guess_this_cycle.value) = seg_t_zero;

        seg_t *value_ptr_copy_a;
        seg_t value_header_b;
        seg_t tmp;
        seg_t *border;
        seg_t *value_ptr_rt;
        seg_t carry;
        while (big_integer_comp_u(&copy_a, b) > -1)
        {
            carry = seg_t_zero;
            value_header_b = *((b->value) + ((b->used_segments) - 1ul));
            value_ptr_copy_a = (copy_a.value) + ((copy_a.used_segments) - 1ul);
            border = copy_a.value - 2u + (b->used_segments);
            value_ptr_rt = guess_this_cycle.value + (copy_a.used_segments - (b->used_segments));
            guess_this_cycle.used_segments = (copy_a.used_segments - (b->used_segments) + 1ul);
            for (; value_ptr_copy_a > border; value_ptr_copy_a--, value_ptr_rt--)
            {
                tmp = ((*value_ptr_copy_a) + carry * SEGMENT_MOD);
                *value_ptr_rt = tmp / value_header_b;
                carry = tmp % value_header_b;
            }
            value_ptr_rt = guess_this_cycle.value + (copy_a.used_segments - (b->used_segments));
            while (*value_ptr_rt == 0 && value_ptr_rt > guess_this_cycle.value)
            {
                (guess_this_cycle.used_segments)--;
                value_ptr_rt--;
            }
            big_integer_mul_u_noalloc(&buf, b, &guess_this_cycle, buf.total_segments);
            int tmp_rt_0 = big_integer_comp_u(&copy_a, &buf);
            if (big_integer_comp_u(&copy_a, &buf) == -1)
            {
                carry = seg_t_zero;
                value_header_b++;
                value_ptr_copy_a = copy_a.value + (copy_a.used_segments - 1ul);
                border = copy_a.value - 2u + (b->used_segments);
                value_ptr_rt = guess_this_cycle.value + (copy_a.used_segments - (b->used_segments));
                guess_this_cycle.used_segments = (copy_a.used_segments - (b->used_segments) + 1ul);
                for (; value_ptr_copy_a > border; value_ptr_copy_a--, value_ptr_rt--)
                {
                    tmp = ((*value_ptr_copy_a) + carry * SEGMENT_MOD);
                    *value_ptr_rt = tmp / value_header_b;
                    carry = tmp % value_header_b;
                }
                value_ptr_rt = guess_this_cycle.value + (copy_a.used_segments - (b->used_segments));
                while (*value_ptr_rt == 0 && value_ptr_rt > guess_this_cycle.value)
                {
                    (guess_this_cycle.used_segments)--;
                    value_ptr_rt--;
                }
                big_integer_mul_u_noalloc(&buf, b, &guess_this_cycle, buf.total_segments);
            }
            int tmp_rt_1 = big_integer_comp_u(&copy_a, &buf);
            big_integer_sub_u_noalloc(&copy_a, &copy_a, &buf, copy_a.total_segments);
            int tmp_rt = big_integer_comp_u(&copy_a, b);
            big_integer_add_u_noalloc(dst, dst, &guess_this_cycle, total_seg);
        }
        if ((a->total_segments) >= STATIC_BUF_SEG_LEN)
        {
            big_integer_destory(&copy_a);
            big_integer_destory(&buf);
            big_integer_destory(&guess_this_cycle);
        }
    }
    return (dst->used_segments);
}

seg_nums_t big_integer_div_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    seg_nums_t rt = big_integer_div_u_noalloc(dst, a, b, total_seg);
    if ((dst->used_segments) == 1ul && *(dst->value) == seg_t_zero)
    {
        (dst->flag) = POSITIVE;
    }
    else
    {
        (dst->flag) = ((a->flag) == (b->flag)) ? POSITIVE : NEGATIVE;
    }
    return rt;
}

big_integer big_integer_div_u(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    int comp_u_rt = big_integer_comp_u(a, b);
    if (comp_u_rt == 1)
    {
        rt.total_segments = (a->used_segments);
    }
    else
    {
        rt.total_segments = 1ul;
    }
    rt.value = (seg_t *)calloc(rt.total_segments, SEG_MEM_WIDTH);
    big_integer_div_u_noalloc(&rt, a, b, rt.total_segments);
    big_integer_shrink(&rt);
    return rt;
}

seg_nums_t big_integer_mod_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    (dst->flag) = POSITIVE;
    (dst->total_segments) = total_seg;
    big_integer div_rt;
    int comp_u_div = big_integer_comp_u(a, b);
    if (comp_u_div < 0)
    {
        if ((dst->value) != (a->value))
        {
            *dst = big_integer_copy(a, dst->value, total_seg);
        }
        return (dst->used_segments);
    }
    else if (comp_u_div == 0)
    {
        (dst->used_segments) = 1ul;
        *(dst->value) = seg_t_zero;
        return (dst->used_segments);
    }
    else
    {
        div_rt.total_segments = (a->used_segments);
        div_rt.value = (seg_t *)calloc(div_rt.total_segments, SEG_MEM_WIDTH);
        big_integer_div_u_noalloc(&div_rt, a, b, div_rt.total_segments);
        big_integer mul_rt;
        mul_rt.total_segments = (a->used_segments);
        mul_rt.value = (seg_t *)calloc(mul_rt.total_segments, SEG_MEM_WIDTH);
        big_integer_mul_u_noalloc(&mul_rt, b, &div_rt, mul_rt.total_segments);
        big_integer_sub_u_noalloc(dst, a, &mul_rt, total_seg);
        big_integer_destory(&div_rt);
        big_integer_destory(&mul_rt);
        return (dst->used_segments);
    }
}

seg_nums_t big_integer_mod_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const seg_nums_t total_seg)
{
    seg_nums_t rt = big_integer_mod_u_noalloc(dst, a, b, total_seg);
    if (a->flag)
    {
        rt = big_integer_sub_u_noalloc(dst, b, dst, total_seg);
    }
    return rt;
}

big_integer big_integer_mod_u(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    rt.flag = POSITIVE;
    big_integer div_rt;
    int comp_u_div = big_integer_comp_u(a, b);
    if (comp_u_div < 0)
    {
        rt.value = (seg_t *)calloc((a->used_segments), SEG_MEM_WIDTH);
        rt.total_segments = (a->used_segments);
        rt = big_integer_copy(a, rt.value, rt.total_segments);
        return rt;
    }
    else if (comp_u_div == 0)
    {
        rt.used_segments = 1ul;
        rt.value = (seg_t *)calloc((a->used_segments), SEG_MEM_WIDTH);
        rt.total_segments = 1ul;
        *(rt.value) = seg_t_zero;
        return rt;
    }
    else
    {
        rt.value = (seg_t *)calloc((b->used_segments), SEG_MEM_WIDTH);
        rt.total_segments = (b->used_segments);
        big_integer_mod_u_noalloc(&rt, a, b, rt.total_segments);
        big_integer_shrink(&rt);
        return rt;
    }
}

seg_nums_t big_integer_exp_u_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const big_integer *const mod, const seg_nums_t total_seg)
{
    const static seg_t mask = 0x1ul;
    (dst->used_segments) = 1ul;
    *(dst->value) = 1ul;
    if ((b->used_segments) == 1ul && *(b->value) == seg_t_zero)
    {
        return (dst->used_segments);
    }
    else
    {
        big_integer guess;
        big_integer guess_this_cycle;
        if (STATIC_BUF_SEG_LEN > (a->used_segments) && STATIC_BUF_SEG_LEN - (a->used_segments) > (a->used_segments))
        {
            guess.total_segments = STATIC_BUF_SEG_LEN;
            guess = big_integer_copy(a, shared_buf_1, STATIC_BUF_SEG_LEN);
            guess_this_cycle = big_integer_copy(a, shared_buf_2, STATIC_BUF_SEG_LEN);
        }
        else
        {
            guess.total_segments = (a->used_segments) + (a->used_segments);
            guess.value = (seg_t *)calloc(guess.total_segments, SEG_MEM_WIDTH);
            guess = big_integer_copy(a, guess.value, guess.total_segments);
            guess_this_cycle.total_segments = (a->used_segments) + (a->used_segments);
            guess_this_cycle.value = (seg_t *)calloc(guess_this_cycle.total_segments, SEG_MEM_WIDTH);
            guess_this_cycle = big_integer_copy(a, guess_this_cycle.value, guess_this_cycle.total_segments);
        }
        seg_nums_t curr_seg;
        seg_t seg_bit;
        for (curr_seg = 0ul; curr_seg < (b->used_segments) - 1ul; curr_seg++)
        {
            for (seg_bit = 0ul; seg_bit < SEG_BITS_WIDTH; seg_bit++)
            {
                if (*((b->value) + curr_seg) & (mask << seg_bit))
                {
                    big_integer_mul_u_noalloc(&guess_this_cycle, &guess, dst, guess_this_cycle.total_segments);
                    big_integer_mod_u_noalloc(dst, &guess_this_cycle, mod, total_seg);
                }
                big_integer_mul_u_noalloc(&guess_this_cycle, &guess, &guess, guess_this_cycle.total_segments);
                big_integer_mod_u_noalloc(&guess, &guess_this_cycle, mod, guess.total_segments);
            }
        }
        for (seg_bit = 0ul; seg_bit < SEG_BITS_WIDTH; seg_bit++)
        {
            if (*((b->value) + curr_seg) & (mask << seg_bit))
            {
                big_integer_mul_u_noalloc(&guess_this_cycle, &guess, dst, guess_this_cycle.total_segments);
                big_integer_mod_u_noalloc(dst, &guess_this_cycle, mod, total_seg);
            }
            if (*((b->value) + curr_seg) / (mask << seg_bit))
            {
                big_integer_mul_u_noalloc(&guess_this_cycle, &guess, &guess, guess_this_cycle.total_segments);
                big_integer_mod_u_noalloc(&guess, &guess_this_cycle, mod, guess.total_segments);
            }
        }
        if (STATIC_BUF_SEG_LEN > (a->used_segments) && STATIC_BUF_SEG_LEN - (a->used_segments) > (a->used_segments))
        {
            big_integer_destory(&guess);
            big_integer_destory(&guess_this_cycle);
        }
        return (dst->used_segments);
    }
}
seg_nums_t big_integer_exp_noalloc(big_integer *const dst, const big_integer *const a, const big_integer *const b, const big_integer *const mod, const seg_nums_t total_seg)
{
    big_integer_exp_u_noalloc(dst, a, b, mod, total_seg);
    if ((b->used_segments) > seg_nums_zero && *(b->value) % 2ul == 1)
    {
        (dst->flag) = NEGATIVE;
    }
    return (dst->used_segments);
}

seg_nums_t big_integer_init(big_integer *const dst, const void *const src, const char mode, size_t bytes)
{
    switch (mode)
    {
    case FROM_CHARS:
        big_integer_init_from_chars(dst, (char *)src);
        break;
    case FROM_BYTES:
        big_integer_init_from_bytes(dst, src, bytes);
    default:
        return 0;
    }
    while ((dst->value)[(dst->used_segments) - 1ul] == 0ul && (dst->used_segments) > 1ul)
    {
        (dst->used_segments--);
    }
    return (dst->used_segments);
}

int big_integer_destory(const big_integer *const data)
{
    free(data->value);
}

seg_nums_t big_integer_write(int fd, const big_integer *const data)
{
    if (data->flag)
    {
        write(fd, "-", sizeof(char));
    }
    static char out_buf[STR_SEG_WIDTH + 2u];
    memset(out_buf, 0, STR_SEG_WIDTH + 2u);
    seg_nums_t cycle = 0;
    seg_t *value_ptr = (data->value) + (data->used_segments) - 1u;
    size_t n = sprintf(out_buf, "%lX", *(value_ptr - cycle));
    write(fd, out_buf, n);
    for (cycle = 1; cycle < (data->used_segments); cycle++)
    {
        sprintf(out_buf, "%0*lX", (int)STR_SEG_WIDTH, *(value_ptr - cycle));
        write(fd, out_buf, STR_SEG_WIDTH);
    }
    return cycle;
}

int big_integer_comp(const big_integer *const a, big_integer *b)
{
    if ((a->flag) == (b->flag))
    {
        int comp = big_integer_comp_u(a, b);
        return (a->flag) ? -comp : comp;
    }
    else
    {
        return (a->flag) ? -1 : 1;
    }
}
big_integer big_integer_add(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    if ((a->flag) == (b->flag))
    {
        rt = big_integer_add_u(a, b);
        rt.flag = (a->flag);
    }
    else if (b->flag)
    {
        rt = big_integer_sub_u(a, b);
    }
    else
    {
        rt = big_integer_sub_u(b, a);
    }
    big_integer_shrink(&rt);
    return rt;
}
big_integer big_integer_sub(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    if ((a->flag) != (b->flag))
    {
        rt = big_integer_add_u(a, b);
        rt.flag = a->flag;
    }
    else if (a->flag)
    {
        rt = big_integer_sub_u(b, a);
    }
    else
    {
        rt = big_integer_sub_u(a, b);
    }
    big_integer_shrink(&rt);
    return rt;
}
big_integer big_integer_mul(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    rt = big_integer_mul_u(a, b);
    if (rt.used_segments == 1ul && *(rt.value) == seg_t_zero)
    {
        rt.flag = POSITIVE;
    }
    else
    {
        rt.flag = ((a->flag) == (b->flag)) ? POSITIVE : NEGATIVE;
    }
    return rt;
}
big_integer big_integer_div(const big_integer *const a, const big_integer *const b)
{
    big_integer rt;
    rt = big_integer_div_u(a, b);
    if (rt.used_segments == 1ul && *(rt.value) == seg_t_zero)
    {
        rt.flag = POSITIVE;
    }
    else
    {
        rt.flag = ((a->flag) == (b->flag)) ? POSITIVE : NEGATIVE;
    }
    return rt;
}
big_integer big_integer_mod(const big_integer *const a, const big_integer *const b)
{
    big_integer rt = big_integer_mod_u(a, b);
    if (a->flag)
    {
        seg_nums_t len = ((a->used_segments) > (b->used_segments)) ? (a->total_segments) : (b->total_segments);
        big_integer re = big_integer_sub_u(b, &rt);
        big_integer_shrink(&re);
        big_integer_destory(&rt);
        rt = re;
    }
    else
    {
        big_integer_shrink(&rt);
    }
    return rt;
}
seg_nums_t big_integer_increase(big_integer *const a, const big_integer *const b)
{
    if ((a->used_segments) <= (b->used_segments))
    {
        (a->value) = (seg_t *)realloc((a->value), (b->used_segments) + 1ul);
        (a->total_segments) = (b->used_segments) + 1ul;
    }
    else
    {
        (a->value) = (seg_t *)realloc((a->value), (a->used_segments) + 1ul);
        (a->total_segments) = (a->used_segments) + 1ul;
    }
    seg_nums_t rt = big_integer_add_noalloc(a, a, b, (a->total_segments));
    big_integer_shrink(a);
    return rt;
}

big_integer big_integer_exp_u(const big_integer *const a, const big_integer *const b, const big_integer *const mod)
{
    big_integer rt;
    rt.total_segments = (mod->total_segments) + 1ul;
    rt.value = (seg_t *)calloc(rt.total_segments, SEG_MEM_WIDTH);
    big_integer_exp_u_noalloc(&rt, a, b, mod, rt.total_segments);
    big_integer_shrink(&rt);
    return rt;
}

big_integer big_integer_exp(const big_integer *const a, const big_integer *const b, const big_integer *const mod)
{
    big_integer rt = big_integer_exp_u(a, b, mod);
    if ((b->used_segments) > seg_nums_zero && *(b->value) % 2ul == 1)
    {
        rt.flag = NEGATIVE;
    }
    return rt;
}

seg_nums_t big_integer_to_str(char *const dst, const big_integer *const obj, size_t str_len)
{
    if (str_len == 0ul)
    {
        return -1;
    }
    else if ((obj->used_segments) > str_len / STR_SEG_WIDTH)
    {
        return 0;
    }
    else
    {
        seg_nums_t cycle = 0;
        seg_t *value_ptr = (obj->value) + (obj->used_segments) - 1u;
        size_t n = sprintf(dst, "%s%lX", (obj->flag) ? "-" : "", *(value_ptr - cycle));
        char *str_ptr = dst + n;
        for (cycle = 1; cycle < (obj->used_segments); cycle++)
        {
            sprintf(str_ptr, "%0*lX", (int)STR_SEG_WIDTH, *(value_ptr - cycle));
        }
        return (obj->used_segments);
    }
}