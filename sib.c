/****************************
 * Andrew Guinn
 * Project 1
 * sib.c
 ****************************/
#include "sib.h"

#define SCALE_MASK 0xC0

#define SCALE_SHIFT 6
#define INDEX_SHIFT 3
#define BASE_SHIFT 0

int sib_get_scale(unsigned char uchar)
{
    uchar = (uchar & SCALE_MASK) >> SCALE_SHIFT;

    switch(uchar) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 4;
    case 3:
        return 8;
    default:
        return -1;
    }
}

reg_t sib_get_index(unsigned char uchar)
{
    return register_uchar_sect_to_reg(uchar, INDEX_SHIFT);
}

reg_t sib_get_base(unsigned char uchar)
{
    return register_uchar_sect_to_reg(uchar, BASE_SHIFT);
}

bool sib_need_sib(op_t op, mod_t mod, reg_t rm)
{
    return op_has_modrm(op) == OP_BOOL_YES && mod != direct && rm == esp;
}
