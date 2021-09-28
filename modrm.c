/****************************
 * Andrew Guinn
 * Project 1
 * modrm.c
 ****************************/
#include "modrm.h"

#define MOD_MASK 0xC0

#define MOD_SHIFT 6
#define REG_SHIFT 3
#define RM_SHIFT 0

mod_t modrm_get_mod(unsigned char uchar)
{
    uchar = (uchar & MOD_MASK) >> MOD_SHIFT;

    switch(uchar) {
    case 0:
        return mem_access;
    case 1:
        return byte_disp;
    case 2:
        return dword_disp;
    case 3:
        return direct;
    default:
        return err_mod;
    }
}

reg_t modrm_get_reg(unsigned char uchar)
{
    return register_uchar_sect_to_reg(uchar, REG_SHIFT);
}

reg_t modrm_get_rm(unsigned char uchar)
{
    return register_uchar_sect_to_reg(uchar, RM_SHIFT);
}

bool modrm_has_displacement(mod_t mod, reg_t rm)
{
    return mod == byte_disp || mod == dword_disp ||
           modrm_is_special_displacement_mod(mod, rm);
}

bool modrm_is_special_displacement_mod(mod_t mod, reg_t rm)
{
    return mod == mem_access && rm == ebp;
}

int modrm_get_displacement_size(mod_t mod, reg_t rm)
{
    switch(mod) {
    case mem_access:
        if (modrm_is_special_displacement_mod(mod, rm)) {
            return 4;
        }

        break;
    case byte_disp:
        return 1;
    case dword_disp:
        return 4;
    default:
        break;
    }

    return 0;
}
