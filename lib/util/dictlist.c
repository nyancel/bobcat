#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>

#include "./dictlist.h"

size_t dln_length(struct dictlist_node *dl)
{
    struct dictlist_node *current = dl;
    while (current->prev != NULL)
    {
        current = current->prev;
    }

    size_t length = 1;
    while (current->next != NULL)
    {
        length++;
        current = current->next;
    }

    return length;
}

struct dictlist_node *dln_new(char *name, void *data)
{
    struct dictlist_node *dln = malloc((sizeof(struct dictlist_node)));
    if (dln == NULL)
    {
        perror("dln_new: malloc returned null");
        return NULL;
    }

    dln->name = name;
    dln->data = data;
    dln->next = NULL;
    dln->prev = NULL;
    return dln;
}

int dln_push(struct dictlist_node *dl, char *name, void *data)
{
    if (dl == NULL)
    {
        perror("dln_push: dl = NULL");
        return -1;
    }

    // create new data
    struct dictlist_node *new = dln_new(name, data);
    // walk to eol
    struct dictlist_node *end = dl;
    while (end->next != NULL)
    {
        end = end->next;
    }
    // fix the pointers
    end->next = new;
    new->prev = end;
    return 0;
}

struct dictlist_node *dln_pop(struct dictlist_node *dl)
{
    // just return the item as is if its a single node
    size_t l = dln_length(dl);
    if (l == 1)
    {
        return dl;
    }

    struct dictlist_node *end = dl;
    while (end->next != NULL)
    {
        end = end->next;
    }
    // disconnect the end from the list
    end->prev->next = NULL;
    end->prev = NULL;
    return end;
}

void *dln_get(struct dictlist_node *dl, char *name)
{
    // find the start of the list;
    struct dictlist_node *current = dl;
    while (current->prev != NULL)
    {
        current = current->prev;
    }

    // then search until the end
    while (1)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current->data;
        }

        if (current->next == NULL)
        {
            perror("dln_get: no name matched");
            return NULL;
        }
        current = current->next;
    }
}

int dln_free(struct dictlist_node *dl)
{
    // find the end of the list;
    struct dictlist_node *current = dl;
    while (current->next != NULL)
    {
        current = current->next;
    }

    // then start deleteing backwards
    while (current->prev != NULL)
    {
        current = current->prev; // standing at n+1
        free(current->next);
        current->next = NULL;
    }
    free(current);
    return 0;
}
