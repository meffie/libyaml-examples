/*
 * Example application data structures.
 */

#include <stdbool.h>

struct fruit {
    struct fruit *next;
    char *name;
    char *color;
    int count;
    struct variety *varieties;
};

struct variety {
    struct variety *next;
    char *name;
    char *color;
    bool seedless;
};

void bail(const char *msg);
void *bail_alloc(size_t size);
char *bail_strdup(const char *s);

void add_fruit(struct fruit **fruits, char *name, char *color, int count, struct variety *varieties);
void add_variety(struct variety **variety, char *name, char *color, bool seedless);

void destroy_fruits(struct fruit **fruits);
void destroy_varieties(struct variety **varieties);
