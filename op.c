/****************************
 * Andrew Guinn
 * Project 1
 * op.c
 ****************************/
#include <string.h>
#include <stdlib.h>

#include "op.h"

#define OP_ADD_STR "add"
#define OP_AND_STR "and"
#define OP_CALL_STR "call"
#define OP_CLFLUSH_STR "clflush"
#define OP_CMP_STR "cmp"
#define OP_DEC_STR "dec"
#define OP_IDIV_STR "idiv"
#define OP_IMUL_STR "imul"
#define OP_INC_STR "inc"
#define OP_JMP_STR "jmp"
#define OP_JZ_STR "jz"
#define OP_JNZ_STR "jnz"
#define OP_LEA_STR "lea"
#define OP_MOV_STR "mov"
#define OP_MOVSD_STR "movsd"
#define OP_MUL_STR "mul"
#define OP_NEG_STR "neg"
#define OP_NOP_STR "nop"
#define OP_NOT_STR "not"
#define OP_OR_STR "or"
#define OP_OUT_STR "out"
#define OP_POP_STR "pop"
#define OP_PUSH_STR "push"
#define OP_CMPSD_STR "cmpsd"
#define OP_RETF_STR "retf"
#define OP_RETN_STR "retn"
#define OP_SAL_STR "sal"
#define OP_SAR_STR "sar"
#define OP_SHR_STR "shr"
#define OP_SBB_STR "sbb"
#define OP_SUB_STR "sub"
#define OP_TEST_STR "test"
#define OP_XOR_STR "xor"
#define ERR_OP_STR "ERR"
#define OP_AMBIG_STR "UNSURE"

op_t op_uchar_to_op(unsigned char uchar)
{
    switch(uchar) {
    case 0x01:
        return addm;
    case 0x03:
        return addr;
    case 0x05:
        return addd;
    case 0x09:
        return orm;
    case 0x0B:
        return orr;
    case 0x0D:
        return ord;
    case 0x0F:
        return clflush_imul_jz_jnz;
    case 0x19:
        return sbbm;
    case 0x1B:
        return sbbr;
    case 0x1D:
        return sbbi;
    case 0x21:
        return andm;
    case 0x23:
        return andr;
    case 0x25:
        return andd;
    case 0x29:
        return subm;
    case 0x2B:
        return subr;
    case 0x2D:
        return subi;
    case 0x31:
        return xorm;
    case 0x33:
        return xorr;
    case 0x35:
        return xord;
    case 0x39:
        return cmpm;
    case 0x3B:
        return cmpr;
    case 0x3D:
        return cmpd;
    case 0x40 ... 0x47:
        return incr;
    case 0x48 ... 0x4F:
        return decr;
    case 0x50 ... 0x57:
        return pushr;
    case 0x58 ... 0x5F:
        return popr;
    case 0x68:
        return pushi;
    case 0x69:
        return imuli;
    case 0x74:
        return jz8;
    case 0x75:
        return jnz8;
    case 0x81:
        return add_and_cmp_or_sbb_sub_xor;
    case 0x85:
        return testm;
    case 0x89:
        return movm;
    case 0x8B:
        return movr;
    case 0x8D:
        return lea;
    case 0x8F:
        return pop_u;
    case 0x90:
        return nop;
    case 0xA5:
        return movsd;
    case 0xA7:
        return cmpsd;
    case 0xA9:
        return testd;
    case 0xB8 ... 0xBF:
        return movd;
    case 0xC2:
        return retni;
    case 0xC3:
        return retn;
    case 0xC7:
        return mov_u;
    case 0xCA:
        return retfi;
    case 0xCB:
        return retf;
    case 0xD1:
        return sal_sar_shr;
    case 0xE7:
        return out;
    case 0xE8:
        return callr;
    case 0xE9:
        return jmpr;
    case 0xEB:
        return jmp8;
    case 0xF7:
        return idiv_imul_mul_neg_not_test;
    case 0xFF:
        return inc_dec_call_jmp_push;
    default:
        return err_op;
    }
}

op_t op_uchar_uchar_to_op(unsigned char op1, unsigned char op2)
{
    op_t one_op = op_uchar_to_op(op1);

    switch(one_op) {
    case clflush_imul_jz_jnz:
        switch(op2) {
        case 0x84:
            return jz32;
        case 0x85:
            return jnz32;
        case 0xAE:
            return clflush_u;
        case 0xAF:
            return imulr;
        default:
            return err_op;
        }
    default:
        return err_op;
    }
}

op_t op_op_modrm_to_op(op_t op, reg_t modrm_reg_field)
{
    switch(op) {
    case add_and_cmp_or_sbb_sub_xor:
        switch(modrm_reg_field) {
        case eax:
            return addi;
        case ecx:
            return ori;
        case edx:
            return err_op; // We don't handle a 2 case
        case ebx:
            return sbbi;
        case esp:
            return andi;
        case ebp:
            return subi;
        case esi:
            return xori;
        case edi:
            return cmpi;
        }
    case sal_sar_shr:
        switch(modrm_reg_field) {
        case esp:
            return sal;
        case ebp:
            return shr;
        case edi:
            return sar;
        default:
            return err_op; // All other cases unhandled
        }
    case idiv_imul_mul_neg_not_test:
        switch(modrm_reg_field) {
        case eax:
            return testi;
        case ecx:
            return err_op; //We don't handle the 1 case
        case edx:
            return not;
        case ebx:
            return neg;
        case esp:
            return mul;
        case ebp:
            return imulm;
        case esi:
            return err_op; //We don't handle the 6 case
        case edi:
            return idiv;
        }
    case inc_dec_call_jmp_push:
        switch(modrm_reg_field) {
        case eax:
            return incm;
        case ecx:
            return decm;
        case edx:
            return callm;
        case ebx:
            return err_op; //We don't handle the 3 case
        case esp:
            return jmpm;
        case ebp:
            return imulm; //We don't handle the 5 case
        case esi:
            return pushm;
        case edi:
            return err_op; //We don't handle the 7 case
        }
    case clflush_u:
        if (modrm_reg_field == edi) {
            return clflush;
        } else {
            return err_op;
        }
    case pop_u:
        if (modrm_reg_field == eax) {
            return popm;
        } else {
            return err_op;
        }
    case mov_u:
        if (modrm_reg_field == eax) {
            return movi;
        } else {
            return err_op;
        }
    default:
        return err_op;
    }
}

op_t op_uchar_modrm_to_op(unsigned char op, reg_t modrm_reg_field)
{
    op_t one_op = op_uchar_to_op(op);

    return op_op_modrm_to_op(one_op, modrm_reg_field);
}

op_t op_uchar_uchar_modrm_to_op(unsigned char op1, unsigned char op2, reg_t modrm_reg_field)
{
    op_t two_op = op_uchar_uchar_to_op(op1, op2);

    return op_op_modrm_to_op(two_op, modrm_reg_field);
}

char *op_to_str(op_t op)
{
    char *op_str, *buf;

    switch(op) {
    case addd ... addr:
        op_str = OP_ADD_STR;
        break;
    case andd ... andr:
        op_str = OP_AND_STR;
        break;
    case callr ... callm:
        op_str = OP_CALL_STR;
        break;
    case clflush:
        op_str = OP_CLFLUSH_STR;
        break;
    case cmpd ... cmpr:
        op_str = OP_CMP_STR;
        break;
    case decm ... decr:
        op_str = OP_DEC_STR;
        break;
    case idiv:
        op_str = OP_IDIV_STR;
        break;
    case imulm ... imuli:
        op_str = OP_IMUL_STR;
        break;
    case incm ... incr:
        op_str = OP_INC_STR;
        break;
    case jmp8 ... jmpm:
        op_str = OP_JMP_STR;
        break;
    case jz8 ... jz32:
        op_str = OP_JZ_STR;
        break;
    case jnz8 ... jnz32:
        op_str = OP_JNZ_STR;
        break;
    case lea:
        op_str = OP_LEA_STR;
        break;
    case movd ... movr:
        op_str = OP_MOV_STR;
        break;
    case movsd:
        op_str = OP_MOVSD_STR;
        break;
    case mul:
        op_str = OP_MUL_STR;
        break;
    case neg:
        op_str = OP_NEG_STR;
        break;
    case nop:
        op_str = OP_NOP_STR;
        break;
    case not:
        op_str = OP_NOT_STR;
        break;
    case ord ... orr:
        op_str = OP_OR_STR;
        break;
    case out:
        op_str = OP_OUT_STR;
        break;
    case popm ... popr:
        op_str = OP_POP_STR;
        break;
    case pushm ... pushi:
        op_str = OP_PUSH_STR;
        break;
    case cmpsd:
        op_str = OP_CMPSD_STR;
        break;
    case retf ... retfi:
        op_str = OP_RETF_STR;
        break;
    case retn ... retni:
        op_str = OP_RETN_STR;
        break;
    case sal:
        op_str = OP_SAL_STR;
        break;
    case sar:
        op_str = OP_SAR_STR;
        break;
    case shr:
        op_str = OP_SHR_STR;
        break;
    case sbbd ... sbbr:
        op_str = OP_SBB_STR;
        break;
    case subd ... subm:
        op_str = OP_SUB_STR;
        break;
    case testd ... testm:
        op_str = OP_TEST_STR;
        break;
    case xord ... xorr:
        op_str = OP_XOR_STR;
        break;
    case clflush_u ... clflush_imul_jz_jnz:
        op_str = OP_AMBIG_STR;
        break;
    case err_op:
        op_str = ERR_OP_STR;
        break;
    default:
        op_str = ERR_OP_STR;
        break;
    }

    buf = calloc(strlen(op_str) + 1, 1);
    if (buf == NULL) {
        goto end;
    }

    memcpy(buf, op_str, strlen(op_str));

end:
    return buf;
}

int op_need_second_byte(op_t op)
{
    switch(op) {
    case clflush:
        return OP_BOOL_YES;
    case imulr:
        return OP_BOOL_YES;
    case jz32:
        return OP_BOOL_YES;
    case jnz32:
        return OP_BOOL_YES;
    case clflush_u:
        return OP_BOOL_YES;
    case clflush_imul_jz_jnz:
        return OP_BOOL_YES;
    default:
        return OP_BOOL_NO;
    }
}

int op_need_modrm(op_t op)
{
    switch(op) {
    case add_and_cmp_or_sbb_sub_xor:
        return OP_BOOL_YES;
    case sal_sar_shr:
        return OP_BOOL_YES;
    case idiv_imul_mul_neg_not_test:
        return OP_BOOL_YES;
    case inc_dec_call_jmp_push:
        return OP_BOOL_YES;
    case clflush_u:
        return OP_BOOL_YES;
    case mov_u:
        return OP_BOOL_YES;
    case pop_u:
        return OP_BOOL_YES;
    case addi:
        return OP_BOOL_YES;
    case andi:
        return OP_BOOL_YES;
    case callm:
        return OP_BOOL_YES;
    case clflush:
        return OP_BOOL_YES;
    case cmpi:
        return OP_BOOL_YES;
    case decm:
        return OP_BOOL_YES;
    case idiv:
        return OP_BOOL_YES;
    case imulm:
        return OP_BOOL_YES;
    case incm:
        return OP_BOOL_YES;
    case jmpm:
        return OP_BOOL_YES;
    case movi:
        return OP_BOOL_YES;
    case ori:
        return OP_BOOL_YES;
    case popm:
        return OP_BOOL_YES;
    case pushm:
        return OP_BOOL_YES;
    case sal:
        return OP_BOOL_YES;
    case sar:
        return OP_BOOL_YES;
    case shr:
        return OP_BOOL_YES;
    case sbbi:
        return OP_BOOL_YES;
    case subi:
        return OP_BOOL_YES;
    case testi:
        return OP_BOOL_YES;
    case xori:
        return OP_BOOL_YES;
    default:
        return OP_BOOL_NO;
    }
}

int op_has_modrm(op_t op)
{
    switch(op) {
    case addi ... addr:
        return OP_BOOL_YES;
    case andi ... andr:
        return OP_BOOL_YES;
    case callm ... clflush:
        return OP_BOOL_YES;
    case cmpi ... decm:
        return OP_BOOL_YES;
    case idiv ... incm:
        return OP_BOOL_YES;
    case jmpm:
        return OP_BOOL_YES;
    case lea:
        return OP_BOOL_YES;
    case movi ... movr:
        return OP_BOOL_YES;
    case mul ... neg:
        return OP_BOOL_YES;
    case not:
        return OP_BOOL_YES;
    case ori ... orr:
        return OP_BOOL_YES;
    case popm:
        return OP_BOOL_YES;
    case pushm:
        return OP_BOOL_YES;
    case sal ... shr:
        return OP_BOOL_YES;
    case sbbi ... sbbr:
        return OP_BOOL_YES;
    case subi ... subr:
        return OP_BOOL_YES;
    case testi ... testm:
        return OP_BOOL_YES;
    case xori ... xorr:
        return OP_BOOL_YES;
    case clflush_u:
        return OP_BOOL_YES;
    case mov_u:
        return OP_BOOL_YES;
    case pop_u:
        return OP_BOOL_YES;
    case inc_dec_call_jmp_push ... clflush_imul_jz_jnz:
        return OP_BOOL_MAYBE;
    default:
        return OP_BOOL_NO;
    }
}

int op_get_immediate_size(op_t op)
{
    switch(op) {
    case addd ... addi:
        return 4;
    case andd ... andi:
        return 4;
    case cmpd ...cmpi:
        return 4;
    case imuli:
        return 4;
    case movd ... movi:
        return 4;
    case ord ... ori:
        return 4;
    case out:
        return 1;
    case pushi:
        return 4;
    case retfi:
        return 2;
    case retni:
        return 2;
    case sbbd ... sbbi:
        return 4;
    case subd ... subi:
        return 4;
    case testd ... testi:
        return 4;
    case xord ... xori:
        return 4;
    default:
        return 0;
    }
}

int op_has_immediate(op_t op)
{
    switch(op) {
    case addd ... addi:
        return OP_BOOL_YES;
    case andd ... andi:
        return OP_BOOL_YES;
    case cmpd ...cmpi:
        return OP_BOOL_YES;
    case imuli:
        return OP_BOOL_YES;
    case movd ... movi:
        return OP_BOOL_YES;
    case ord ... ori:
        return OP_BOOL_YES;
    case out:
        return OP_BOOL_YES;
    case pushi:
        return OP_BOOL_YES;
    case retfi:
        return OP_BOOL_YES;
    case retni:
        return OP_BOOL_YES;
    case sbbd ... sbbi:
        return OP_BOOL_YES;
    case subd ... subi:
        return OP_BOOL_YES;
    case testd ... testi:
        return OP_BOOL_YES;
    case xord ... xori:
        return OP_BOOL_YES;
    case idiv_imul_mul_neg_not_test:
        return OP_BOOL_MAYBE;
    case add_and_cmp_or_sbb_sub_xor:
        return OP_BOOL_YES;
    case clflush_imul_jz_jnz:
        return OP_BOOL_MAYBE;
    default:
        return OP_BOOL_NO;
    }
}

int op_has_displacement(op_t op)
{
    switch(op) {
    case callr:
        return OP_BOOL_YES;
    case jmp8 ... jmpr:
        return OP_BOOL_YES;
    case jz8 ... jnz32:
        return OP_BOOL_YES;
    default:
        return OP_BOOL_NO;
    }
}

int op_get_displacement_size(op_t op)
{
    switch(op) {
    case callr:
        return 4;
    case jmp8:
        return 1;
    case jmpr:
        return 4;
    case jz8:
        return 1;
    case jz32:
        return 4;
    case jnz8:
        return 1;
    case jnz32:
        return 4;
    default:
        return 0;
    }
}

int op_op_has_reg(op_t op)
{
    switch(op) {
    case pushr:
        return OP_BOOL_YES;
    case popr:
        return OP_BOOL_YES;
    case decr:
        return OP_BOOL_YES;
    case incr:
        return OP_BOOL_YES;
    case movd:
        return OP_BOOL_YES;
    default:
        return OP_BOOL_NO;
    }
}

reg_t op_op_get_reg(unsigned char uchar)
{
    switch(uchar) {
    case 0x40 ... 0x47:
        return register_uchar_to_reg(uchar - 0x40);
    case 0x48 ... 0x4F:
        return register_uchar_to_reg(uchar - 0x48);
    case 0x50 ... 0x57:
        return register_uchar_to_reg(uchar - 0x50);
    case 0x58 ... 0x5F:
        return register_uchar_to_reg(uchar - 0x58);
    case 0xB8 ... 0xBF:
        return register_uchar_to_reg(uchar - 0xB8);
    default:
        return err_reg;
    }
}

int op_op_arg_polarity(op_t op)
{
    switch(op) {
    case addd:
        return OP_ARG_POL_NONE;
    case addi:
        return OP_ARG_POL_LEFT;
    case addm:
        return OP_ARG_POL_LEFT;
    case addr:
        return OP_ARG_POL_RIGHT;
    case andd:
        return OP_ARG_POL_NONE;
    case andi:
        return OP_ARG_POL_LEFT;
    case andm:
        return OP_ARG_POL_LEFT;
    case andr:
        return OP_ARG_POL_RIGHT;
    case callr:
        return OP_ARG_POL_NONE;
    case callm:
        return OP_ARG_POL_ONLY;
    case clflush:
        return OP_ARG_POL_ONLY;
    case cmpd:
        return OP_ARG_POL_NONE;
    case cmpi:
        return OP_ARG_POL_LEFT;
    case cmpm:
        return OP_ARG_POL_LEFT;
    case cmpr:
        return OP_ARG_POL_RIGHT;
    case decm:
        return OP_ARG_POL_ONLY;
    case decr:
        return OP_ARG_POL_NONE;
    case idiv:
        return OP_ARG_POL_ONLY;
    case imulm:
        return OP_ARG_POL_ONLY;
    case imulr:
        return OP_ARG_POL_RIGHT;
    case imuli:
        return OP_ARG_POL_RIGHT;
    case incm:
        return OP_ARG_POL_ONLY;
    case incr:
        return OP_ARG_POL_NONE;
    case jmp8 ... jmpr:
        return OP_ARG_POL_NONE;
    case jmpm:
        return OP_ARG_POL_ONLY;
    case jz8 ... jnz32:
        return OP_ARG_POL_NONE;
    case lea:
        return OP_ARG_POL_RIGHT;
    case movd:
        return OP_ARG_POL_NONE;
    case movi:
        return OP_ARG_POL_LEFT;
    case movm:
        return OP_ARG_POL_LEFT;
    case movr:
        return OP_ARG_POL_RIGHT;
    case movsd:
        return OP_ARG_POL_NONE;
    case mul:
        return OP_ARG_POL_ONLY;
    case neg:
        return OP_ARG_POL_ONLY;
    case nop:
        return OP_ARG_POL_NONE;
    case not:
        return OP_ARG_POL_ONLY;
    case ord:
        return OP_ARG_POL_NONE;
    case ori:
        return OP_ARG_POL_LEFT;
    case orm:
        return OP_ARG_POL_LEFT;
    case orr:
        return OP_ARG_POL_RIGHT;
    case out:
        return OP_ARG_POL_NONE;
    case popm:
        return OP_ARG_POL_ONLY;
    case popr:
        return OP_ARG_POL_NONE;
    case pushm:
        return OP_ARG_POL_ONLY;
    case pushr ... retni:
        return OP_ARG_POL_NONE;
    case sal ... shr:
        return OP_ARG_POL_LEFT;
    case sbbd:
        return OP_ARG_POL_NONE;
    case sbbi:
        return OP_ARG_POL_LEFT;
    case sbbm:
        return OP_ARG_POL_LEFT;
    case sbbr:
        return OP_ARG_POL_RIGHT;
    case subd:
        return OP_ARG_POL_NONE;
    case subi:
        return OP_ARG_POL_LEFT;
    case subm:
        return OP_ARG_POL_LEFT;
    case subr:
        return OP_ARG_POL_RIGHT;
    case testd:
        return OP_ARG_POL_NONE;
    case testi:
        return OP_ARG_POL_LEFT;
    case testm:
        return OP_ARG_POL_LEFT;
    case xord:
        return OP_ARG_POL_NONE;
    case xori:
        return OP_ARG_POL_LEFT;
    case xorm:
        return OP_ARG_POL_LEFT;
    case xorr:
        return OP_ARG_POL_RIGHT;
    default:
        return OP_ARG_POL_UNSURE;
        break;
    }
}
