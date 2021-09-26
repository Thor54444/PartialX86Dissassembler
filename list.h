/****************************
 * Andrew Guinn
 * Project 1
 * list.h
 ****************************/
#ifndef __LIST_H__
#define __LIST_H__

typedef struct ele_t {
  struct ele_t *next;
  void *value;
} ele_t;

// INITALIZE LIST WITH A CALLOC'D ELEMENT
int list_add_element(ele_t *head, void *new_val);

ele_t *list_get_next(ele_t *ele);

void list_free_list(ele_t *head, void (*free_ele)(void *));

#endif //__LIST_H__
