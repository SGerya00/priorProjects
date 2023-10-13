#ifndef LIST_AND_TREE_H
#define LIST_AND_TREE_H

struct list {
    long long int value;
    struct list *next;
};

int make_new_node(struct list *true_head, long long int v_value);

void delete_next_node(struct list *current);

void clean_sorted_list(const struct list *true_head, size_t counter, size_t *new_counter);

void delete_entire_list(struct list *true_head);

struct tree {
    long long int value;
    struct tree *bottom_left;
    struct tree *bottom_right;
};

struct tree * make_a_leaf(long long int v_value);

void print_tree(const struct tree *big_root);

void delete_leaf(struct tree *big_root);

void delete_entire_tree(struct tree *big_root);

struct tree *make_tree(size_t size_of_sub, struct list *false_head);

#endif //LIST_AND_TREE_H
