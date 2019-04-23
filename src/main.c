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
    big_integer_init(&sample1, "5FA7BD8EDDA2980C8EAC76", FROM_CHARS, 0);
    write(STDOUT_FILENO, "a = ", 4ul);
    big_integer_write(STDOUT_FILENO, &sample1);
    write(STDOUT_FILENO, "\n", sizeof(char));
    big_integer sample2;
    big_integer_init(&sample2, "5FA7BD8EDDA2980C8EAC76", FROM_CHARS, 0);
    write(STDOUT_FILENO, "b = ", 4ul);
    big_integer_write(STDOUT_FILENO, &sample2);
    write(STDOUT_FILENO, "\n", sizeof(char));
    
    big_integer sample3;
    sample3.total_segments = 4ul;
    sample3.value = (seg_t*)calloc(sample3.total_segments, SEG_MEM_WIDTH);
    big_integer_add_noalloc(&sample3, &sample1, &sample2, sample3.total_segments);
    write(STDOUT_FILENO, "c = a + b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample3);
    write(STDOUT_FILENO, "\n", sizeof(char));
    
    big_integer sample4;
    sample4.total_segments = 6ul;
    sample4.value = (seg_t*)calloc(sample4.total_segments, SEG_MEM_WIDTH);
    big_integer_mul_noalloc(&sample4, &sample1, &sample2, sample4.total_segments);
    write(STDOUT_FILENO, "d = a * b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample4);
    write(STDOUT_FILENO, "\n", sizeof(char));

    big_integer sample6;
    sample6.total_segments = 3ul;
    sample6.value = (seg_t*)calloc(sample6.total_segments, SEG_MEM_WIDTH);
    big_integer_sub_noalloc(&sample6, &sample3, &sample1, sample6.total_segments);
    write(STDOUT_FILENO, "z = c - a = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample6);
    write(STDOUT_FILENO, "\n", sizeof(char));

    big_integer sample5;
    sample5.total_segments = 3ul;
    sample5.value = (seg_t*)calloc(sample5.total_segments, SEG_MEM_WIDTH);
    big_integer_div_noalloc(&sample5, &sample4, &sample2, sample5.total_segments);
    write(STDOUT_FILENO, "e = d / b = ", 12ul);
    big_integer_write(STDOUT_FILENO, &sample5);
    write(STDOUT_FILENO, "\n", sizeof(char));
    return 0;
}
