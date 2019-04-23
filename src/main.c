#include "bigInteger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    big_integer sample1;
    big_integer_init(&sample1, "5FA7BD8EDDA2980C8D3", FROM_CHARS, 0);
    write(STDOUT_FILENO, "a = ", 4ul);
    big_integer_write(STDOUT_FILENO, &sample1);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample2;
    big_integer_init(&sample2, "5FA7BD8EDDA2980C8", FROM_CHARS, 0);
    write(STDOUT_FILENO, "b = ", 4ul);
    big_integer_write(STDOUT_FILENO, &sample2);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample6;
    big_integer_init(&sample6, "5FA7BD8EDDA2980C8", FROM_CHARS, 0);
    write(STDOUT_FILENO, "f = ", 4ul);
    big_integer_write(STDOUT_FILENO, &sample6);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample3;
    sample3.total_segments = 6ul;
    sample3.value = (seg_t *)calloc(sample3.total_segments, SEG_MEM_WIDTH);
    big_integer_add_noalloc(&sample3, &sample1, &sample2, sample3.total_segments);
    write(STDOUT_FILENO, "c = a + b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample3);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample4;
    sample4.total_segments = 14ul;
    sample4.value = (seg_t *)calloc(sample4.total_segments, SEG_MEM_WIDTH);
    big_integer_mul_noalloc(&sample4, &sample1, &sample2, sample4.total_segments);
    write(STDOUT_FILENO, "d = a * b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample4);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample5;
    sample5.total_segments = 5ul;
    sample5.value = (seg_t *)calloc(sample5.total_segments, SEG_MEM_WIDTH);
    big_integer_div_noalloc(&sample5, &sample4, &sample2, sample5.total_segments);
    write(STDOUT_FILENO, "e = d / b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample5);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample9;
    sample9.total_segments = 14ul;
    sample9.value = (seg_t *)calloc(sample9.total_segments, SEG_MEM_WIDTH);
    big_integer_mod_u_noalloc(&sample9, &sample1, &sample2, sample9.total_segments);
    write(STDOUT_FILENO, "i = a % b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample9);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample8;
    sample8.total_segments = 8ul;
    sample8.value = (seg_t *)calloc(sample8.total_segments, SEG_MEM_WIDTH);
    big_integer_exp_u_noalloc(&sample8, &sample1, &sample2, &sample6, sample8.total_segments);
    write(STDOUT_FILENO, "h = a ^ b mod f = ", 18ul);
    big_integer_write(STDOUT_FILENO, &sample8);
    write(STDOUT_FILENO, "\n", sizeof(char));
    return 0;
}
