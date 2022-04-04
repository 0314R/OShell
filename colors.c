#include "colors.h"

void red()
{
    printf("\033[1;31m");
}

void brown()
{
    printf("\033[0;33m");
}

void yellow()
{
    printf("\033[1;33m");
}

void green()
{
    printf("\033[0;32m");
}

void blue()
{
    printf("\033[0;34m");
}

void purple()
{
    printf("\033[0;35m");
}

void cyan()
{
    printf("\033[0;36m");
}

void grey()
{
    printf("\033[1;30m");
}

void black()
{
    printf("\033[0;30m");
}

void white()
{
    printf("\033[1;37m");
}

void lightgrey()
{
    printf("\033[0;37m");
}

void lightred()
{
    printf("\033[1;31m");
}

void lightgreen()
{
    printf("\033[1;32m");
}

void lightblue()
{
    printf("\033[1;34m");
}

void lightpurple()
{
    printf("\033[1;35m");
}

void lightcyan()
{
    printf("\033[1;36m");
}

void reset()
{
    printf("\033[0m");
}