#ifndef DICTLIST_H
#define DICTLIST_H

#include <stdlib.h>

struct dictlist_node
{
    struct dictlist_node *next;
    struct dictlist_node *prev;
    char *name;
    void *data;
};

struct dictlist_node *dln_new(char *name, void *data);          // init
int dln_push(struct dictlist_node *dl, char *name, void *data); // append
struct dictlist_node *dln_pop(struct dictlist_node *dl);        // remove and return last
void *dln_get(struct dictlist_node *dl, char *name);            // get only the data
int dln_free(struct dictlist_node *dl);                         // recursively free from head
size_t dln_length(struct dictlist_node *dl);                    // length of list

#endif // DICTLIST_H