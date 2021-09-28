/****************************
 * Andrew Guinn
 * Project 1
 * prefix.c
 ****************************/
#include <string.h>
#include <stdlib.h>

#include "prefix.h"

#define PREFIX_REPNE_STR "repne"
#define PREFIX_ERR_STR "error"

prefix_t prefix_uchar_to_prefix(unsigned char uchar)
{
    switch(uchar) {
    case 0xF2:
        return repne;
    default:
        return err_prefix;
    }
}

char *prefix_to_str(prefix_t prefix)
{
    char *buf, *str;

    switch(prefix) {
    case repne:
        str = PREFIX_REPNE_STR;
        break;
    default:
        str = PREFIX_ERR_STR;
        break;
    }

    buf = calloc(strlen(str + 1), 1);
    if (buf == NULL) {
        goto end;
    }

    memcpy(buf, str, strlen(str));

end:
    return buf;
}
