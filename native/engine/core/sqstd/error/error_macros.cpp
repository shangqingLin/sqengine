#include "./error_macros.h"
#include <stdio.h>

#include <stdio.h>
void _err_print_error(const char *p_function, const char *p_file, int p_line, const char *param, const char *p_error)
{
    if (p_error)
    {
        printf("ERROR: file: %s line: %d param %s msg: %s\n", p_file, p_line, param, p_error);
    }
    else
    {
        printf("ERROR: file: %s line: %d param %s\n", p_file, p_line, param);
    }
}
