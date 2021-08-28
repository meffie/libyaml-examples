/*
 * Example application data structure.
 *
 * Define some simple structures to convert to and from yaml, and some helper
 * functions for our example program.
 *
 * You'll want to handle ENOMEM errors more gracefully in a real program, but
 * to keep our example simple, just bail if we cannot allocate memory.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fruit.h"

/* Helper to bail on error. */
void
bail(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

/* Helper to allocate memory or bail. */
void *
bail_alloc(size_t size)
{
    void *p = calloc(1, size);
    if (!p) {
        bail("out of memory");
    }
    return p;
}

/* Helper to copy a string or bail. */
char *
bail_strdup(const char *s)
{
    char *c = strdup(s ? s : "");
    if (!c) {
        bail("out of memory");
    }
    return c;
}

/* Allocate a "fruit" example object. */
struct fruit *
create_fruit(char *name, char *color, int count)
{
    struct fruit *f = bail_alloc(sizeof(*f));
    f->name = bail_strdup(name);
    f->color = bail_strdup(color);
    f->count = count;
    return f;
}

void
destroy_fruit(struct fruit **pf)
{
    if (*pf) {
        struct fruit *f = *pf;
        free(f->name);
        free(f->color);
        free(*pf);
        *pf = NULL;
    }
}

/*
 * Allocate a "fruit" example and append it to a linked list.
 */
struct fruit *
add_fruit(struct fruits *list, char *name, char *color, int count)
{
    struct fruit *f = create_fruit(name, color, count);
    if (!list->head) {
        /* First one. */
        list->head = f;
        list->tail = f;
    } else {
        /* Append to list. */
        list->tail->next = f;
        list->tail = f;
    }
    return f;
}

/*
 * Free all the "fruits" in the linked list.
 */
void
destroy_fruits(struct fruits *list)
{
    struct fruit *f;
    for (f = list->head; f; f = list->head) {
        list->head = f->next;
        destroy_fruit(&f);
    }
    list->tail = NULL;
}
