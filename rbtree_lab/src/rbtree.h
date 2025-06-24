#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);
void delete_node(rbtree *t, node_t *node);

node_t *rbtree_insert(rbtree *, const key_t);
void rbtree_insert_fixup(rbtree *t, node_t *cur);
void left_rotate(rbtree *t, node_t *x);
void right_rotate(rbtree *t, node_t *x);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_min_subtree(const rbtree *t, node_t *start);
node_t *rbtree_max(const rbtree *);
void rbtree_transplant(rbtree *t, node_t *replaced_node, node_t *substitute_node);
int rbtree_erase(rbtree *, node_t *);
void rbtree_delete_fixup(rbtree *t, node_t *delete_node);
int rbtree_to_array(const rbtree *, key_t *, const size_t);
int rbtree_inorder(const rbtree *t, node_t *node, key_t *arr, const size_t n, size_t *index);
#endif  // _RBTREE_H_
