/*
 * Example application data structure.
 */

struct fruits {
    struct fruit *head;
    struct fruit *tail;
};

struct fruit {
    struct fruit *next;
    char *name;
    char *color;
    int count;
};

/* Helpers */
void bail(const char *msg);
void *bail_alloc(size_t size);
char *bail_strdup(const char *s);

/* Example structure. */
struct fruit *create_fruit(char *name, char *color, int count);
void destroy_fruit(struct fruit **pf);
struct fruit *add_fruit(struct fruits *list, char *name, char *color, int count);
void destroy_fruits(struct fruits *list);
