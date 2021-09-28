/****************************
 * Andrew Guinn
 * Project 1
 * label.c
 ****************************/
#define MAX_LABEL_LENGTH 16

label_t *label_make_label(uint32_t addr, char *label) {
  label_t *label;

  label = malloc(sizeof(label_t));
  if (label) {
    label->addr = addr;
    label->label = calloc(MAX_LABEL_LENGTH + 1, 1);
    if (label->label) {
      memcpy(label->label, label, MAX_LABEL_LENGTH);
    } else {
      free(label);
      label = NULL;
    }
  }

  return label;
}

void free_label(label_t *label) {
  free(label->label);
  free(label);
}
