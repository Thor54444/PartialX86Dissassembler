/****************************
 * Andrew Guinn
 * Project 1
 * Register.h
 ****************************/
#ifndef __REGISTER_H__
#define __REGISTER_H__

typedef enum reg_t {
  eax,
  ecx,
  edx,
  ebx,
  esp,
  ebp,
  esi,
  edi,
  err_reg //Indicates precense of error in a register function
} reg_t;

reg_t register_uchar_to_reg(unsigned char uchar);

reg_t register_uchar_sect_to_reg(unsigned char uchar, int start_bit); 

char *register_reg_to_str(reg_t reg);

#endif //__REGISTER_H__
