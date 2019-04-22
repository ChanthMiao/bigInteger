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
    big_integer sample;
    big_integer_init(&sample, "5FA7BD8EDDA2980C8", FROM_CHARS, 0);
    big_integer_write(STDOUT_FILENO, &sample);
    return 0;
}
