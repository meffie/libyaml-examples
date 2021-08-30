/*
 * Example application data structure.
 *
 * Define some simple structures to convert to and from yaml, and some helper
 * functions for our example program.
 *
 * You'll want to handle ENOMEM errors more gracefully in a real program, but
 * to keep our example simple, we just bail if we cannot allocate memory.
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

void
add_fruit(struct fruit **fruits, char *name, char *color, int count, struct variety *varieties)
{
    /* Create fruit object. */
    struct fruit *f = bail_alloc(sizeof(*f));
    f->name = bail_strdup(name);
    f->color = bail_strdup(color);
    f->count = count;
    f->varieties = varieties;

    /* Append to list. */
    if (!*fruits) {
        *fruits = f;
    } else {
        struct fruit *tail = *fruits;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = f;
    }
}

void
add_variety(struct variety **varieties, char *name, char *color, bool seedless)
{
    /* Create variety object. */
    struct variety *v = bail_alloc(sizeof(*v));
    v->name = bail_strdup(name);
    v->color = bail_strdup(color);
    v->seedless = seedless;

    /* Append to list. */
    if (!*varieties) {
        *varieties = v;
    } else {
        struct variety *tail = *varieties;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = v;
    }
}

void
destroy_fruits(struct fruit **fruits)
{
    for (struct fruit *f = *fruits; f; f = *fruits) {
        *fruits = f->next;
        free(f->name);
        free(f->color);
        destroy_varieties(&f->varieties);
        free(f);
    }
}

void
destroy_varieties(struct variety **varieties)
{
    for (struct variety *v = *varieties; v; v = *varieties) {
        *varieties = v->next;
        free(v->name);
        free(v->color);
        free(v);
    }
}
