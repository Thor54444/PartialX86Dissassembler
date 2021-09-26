/****************************
 * Andrew Guinn
 * Project 1
 * list.c
 ****************************/
#include "list.h"

int list_add_element(ele_t *head, void *new_val) {
  if (head == NULL) {
    return -1;
  } else if (head->value == NULL) {
    head->value = new_val;
    return 0;
  } else if(head->next == NULL) {
    head->next = calloc(sizeof(ele_t), 1);
    if (head->next == NULL) {
      return -1;
    }

    head->next->value = new_val;
    return 0;
  } else {
    list_add_element(head->next, new_val);
  }
}

ele_t *list_get_next(ele_t *ele) {
  if (ele == NULL) {
    return NULL;
  }

  return ele->next;
}

void list_free_list(ele_t *head, void (*free_ele)(void *)) {
  if (head == NULL) {
    return;
  }

  list_free_list(head, free_ele);

  free_ele(head->value);
  free(head);

  return;
}
