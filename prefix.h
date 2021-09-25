/****************************
 * Andrew Guinn
 * Project 1
 * prefix.h
 ****************************/
#ifndef __PREFIX_H__
#define __PREFIX_H__

typedef enum prefix_t {
  repne,
  err_prefix
} prefix_t;

prefix_t prefix_uchar_to_prefix(unsigned char uchar);

char *prefix_to_str(prefix_t prefix);

#endif //__PREFIX_H__
