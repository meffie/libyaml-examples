/*
 * Example libyaml parser.
 *
 * This is a basic example to demonstrate how to convert yaml to c language
 * data objects using the libyaml emitter API.  For details about libyaml, see
 * the libyaml project page http://pyyaml.org/wiki/LibYAML
 *
 * Example yaml data to be parsed:
 *
 *    $ cat fruit.yaml
 *    ---
 *    fruit:
 *    - name: apple
 *      color: red
 *      count: 12
 *      varieties:
 *      - name: macintosh
 *        seedless: false
 *      - name: granny smith
 *        seedless: false
 *    - name: orange
 *      color: orange
 *      count: 3
 *    ...
 *
 * The sequence of yaml events supported is:
 *
 *    <stream>       ::= STREAM-START <document> STREAM-END
 *    <document>     ::= DOCUMENT-START MAPPING-START "fruit" <fruit-list> MAPPING-END DOCUMENT-END
 *    <fruit-list>   ::= SEQUENCE-START <fruit-obj>* SEQUENCE-END
 *    <fruit-obj>    ::= MAPPING-START <fruit-data>* MAPPING-END
 *    <fruit-data>   ::= "name" <string> |
 *                       "color" <string> |
 *                       "count" <integer> |
 *                       "varieties" <variety-list>
 *    <variety-list> ::= SEQUENCE-START <variety-obj>* SEQUENCE-END
 *    <variety-obj>  ::= MAPPING-START <variety-data>* MAPPING-END
 *    <variety-data> ::= "name" <string> |
 *                       "color" <string> |
 *                       "seedless" (0|1) |
 *
 * For example:
 *
 *    $ cat fruit.yaml | ./scan
 *    stream-start-event (1)
 *      document-start-event (3)
 *        mapping-start-event (9)
 *          scalar-event (6) = {value="fruit", length=5}
 *          sequence-start-event (7)
 *            mapping-start-event (9)
 *              scalar-event (6) = {value="name", length=4}
 *              scalar-event (6) = {value="apple", length=5}
 *              scalar-event (6) = {value="color", length=5}
 *              scalar-event (6) = {value="red", length=3}
 *              scalar-event (6) = {value="count", length=5}
 *              scalar-event (6) = {value="12", length=2}
 *              scalar-event (6) = {value="varieties", length=9}
 *              sequence-start-event (7)
 *                mapping-start-event (9)
 *                  scalar-event (6) = {value="name", length=4}
 *                  scalar-event (6) = {value="macintosh", length=9}
 *                  scalar-event (6) = {value="seedless", length=8}
 *                  scalar-event (6) = {value="false", length=5}
 *                mapping-end-event (10)
 *                mapping-start-event (9)
 *                  scalar-event (6) = {value="name", length=4}
 *                  scalar-event (6) = {value="granny smith", length=12}
 *                  scalar-event (6) = {value="seedless", length=8}
 *                  scalar-event (6) = {value="false", length=5}
 *                mapping-end-event (10)
 *              sequence-end-event (8)
 *            mapping-end-event (10)
 *            mapping-start-event (9)
 *              scalar-event (6) = {value="name", length=4}
 *              scalar-event (6) = {value="orange", length=6}
 *              scalar-event (6) = {value="color", length=5}
 *              scalar-event (6) = {value="orange", length=6}
 *              scalar-event (6) = {value="count", length=5}
 *              scalar-event (6) = {value="3", length=1}
 *            mapping-end-event (10)
 *          sequence-end-event (8)
 *        mapping-end-event (10)
 *      document-end-event (4)
 *    stream-end-event (2)
 *
 */
#include <yaml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "fruit.h"

/* Set environment variable DEBUG=1 to enable debug output. */
int debug = 0;

/* yaml_* functions return 1 on success and 0 on failure. */
enum status {
    SUCCESS = 1,
    FAILURE = 0
};

/* Our example parser states. */
enum state {
    STATE_START,    /* start state */
    STATE_STREAM,   /* start/end stream */
    STATE_DOCUMENT, /* start/end document */
    STATE_SECTION,  /* top level */

    STATE_FLIST,    /* fruit list */
    STATE_FVALUES,  /* fruit key-value pairs */
    STATE_FKEY,     /* fruit key */
    STATE_FNAME,    /* fruit name value */
    STATE_FCOLOR,   /* fruit color value */
    STATE_FCOUNT,   /* fruit count value */

    STATE_VLIST,    /* varieties list */
    STATE_VVALUES,  /* variety key-value pairs */
    STATE_VKEY,     /* variety key */
    STATE_VNAME,    /* variety name */
    STATE_VCOLOR,   /* variety color */
    STATE_VSEEDLESS,/* variety seedless */

    STATE_STOP      /* end state */
};

/* Our application parser state data. */
struct parser_state {
    enum state state;      /* The current parse state */
    struct fruit f;        /* Fruit data elements. */
    struct variety v;      /* Variety data elements. */
    struct variety *vlist; /* List of 'variety' objects. */
    struct fruit *flist;   /* List of 'fruit' objects. */
};

/*
 * Convert a yaml boolean string to a boolean value (true|false).
 */
int
get_boolean(const char *string, bool *value)
{
    char *t[] = {"y", "Y", "yes", "Yes", "YES", "true", "True", "TRUE", "on", "On", "ON", NULL};
    char *f[] = {"n", "N", "no", "No", "NO", "false", "False", "FALSE", "off", "Off", "OFF", NULL};
    char **p;

    for (p = t; *p; p++) {
        if (strcmp(string, *p) == 0) {
            *value = true;
            return 0;
        }
    }
    for (p = f; *p; p++) {
        if (strcmp(string, *p) == 0) {
            *value = false;
            return 0;
        }
    }
    return EINVAL;
}


/*
 * Consume yaml events generated by the libyaml parser to
 * import our data into raw c data structures. Error processing
 * is keep to a mimimum since this is just an example.
 */
int consume_event(struct parser_state *s, yaml_event_t *event)
{
    char *value;

    if (debug) {
        printf("state=%d event=%d\n", s->state, event->type);
    }
    switch (s->state) {
    case STATE_START:
        switch (event->type) {
        case YAML_STREAM_START_EVENT:
            s->state = STATE_STREAM;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

     case STATE_STREAM:
        switch (event->type) {
        case YAML_DOCUMENT_START_EVENT:
            s->state = STATE_DOCUMENT;
            break;
        case YAML_STREAM_END_EVENT:
            s->state = STATE_STOP;  /* All done. */
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

     case STATE_DOCUMENT:
        switch (event->type) {
        case YAML_MAPPING_START_EVENT:
            s->state = STATE_SECTION;
            break;
        case YAML_DOCUMENT_END_EVENT:
            s->state = STATE_STREAM;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_SECTION:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            value = (char *)event->data.scalar.value;
            if (strcmp(value, "fruit") == 0) {
               s->state = STATE_FLIST;
            } else {
               fprintf(stderr, "Unexpected scalar: %s\n", value);
               return FAILURE;
            }
            break;
        case YAML_DOCUMENT_END_EVENT:
            s->state = STATE_STREAM;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_FLIST:
        switch (event->type) {
        case YAML_SEQUENCE_START_EVENT:
            s->state = STATE_FVALUES;
            break;
        case YAML_MAPPING_END_EVENT:
            s->state = STATE_SECTION;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_FVALUES:
        switch (event->type) {
        case YAML_MAPPING_START_EVENT:
            s->state = STATE_FKEY;
            break;
        case YAML_SEQUENCE_END_EVENT:
            s->state = STATE_FLIST;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_FKEY:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            value = (char *)event->data.scalar.value;
            if (strcmp(value, "name") == 0) {
                s->state = STATE_FNAME;
            } else if (strcmp(value, "color") == 0) {
                s->state = STATE_FCOLOR;
            } else if (strcmp(value, "count") == 0) {
                s->state = STATE_FCOUNT;
            } else if (strcmp(value, "varieties") == 0) {
                s->state = STATE_VLIST;
            } else {
                fprintf(stderr, "Unexpected key: %s\n", value);
                return FAILURE;
            }
            break;
        case YAML_MAPPING_END_EVENT:
            add_fruit(&s->flist, s->f.name, s->f.color, s->f.count, s->vlist);
            free(s->f.name);
            free(s->f.color);
            memset(&s->f, 0, sizeof(s->f));
            s->vlist = NULL;
            s->state = STATE_FVALUES;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_FNAME:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            if (s->f.name) {
                fprintf(stderr, "Warning: duplicate 'name' key.\n");
                free(s->f.name);
            }
            s->f.name = bail_strdup((char *)event->data.scalar.value);
            s->state = STATE_FKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_FCOLOR:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            if (s->f.color) {
                fprintf(stderr, "Warning: duplicate 'color' key.\n");
                free(s->f.color);
            }
            s->f.color = bail_strdup((char *)event->data.scalar.value);
            s->state = STATE_FKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_FCOUNT:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            s->f.count = atoi((char *)event->data.scalar.value);
            s->state = STATE_FKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_VLIST:
        switch (event->type) {
        case YAML_SEQUENCE_START_EVENT:
            s->state = STATE_VVALUES;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_VVALUES:
        switch (event->type) {
        case YAML_MAPPING_START_EVENT:
            s->state = STATE_VKEY;
            break;
        case YAML_SEQUENCE_END_EVENT:
            s->state = STATE_FKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_VKEY:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            value = (char *)event->data.scalar.value;
            if (strcmp(value, "name") == 0) {
                s->state = STATE_VNAME;
            } else if (strcmp(value, "color") == 0) {
                s->state = STATE_VCOLOR;
            } else if (strcmp(value, "seedless") == 0) {
                s->state = STATE_VSEEDLESS;
            } else {
                fprintf(stderr, "Unexpected key: %s\n", value);
                return FAILURE;
            }
            break;
        case YAML_MAPPING_END_EVENT:
            add_variety(&s->vlist, s->v.name, s->v.color, s->v.seedless);
            free(s->v.name);
            free(s->v.color);
            memset(&s->v, 0, sizeof(s->v));
            s->state = STATE_VVALUES;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_VNAME:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            if (s->v.name) {
                fprintf(stderr, "Warning: duplicate 'name' key.\n");
                free(s->v.name);
            }
            s->v.name = bail_strdup((char *)event->data.scalar.value);
            s->state = STATE_VKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_VCOLOR:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            if (s->v.color) {
                fprintf(stderr, "Warning: duplicate 'color' key.\n");
                free(s->v.color);
            }
            s->v.color = bail_strdup((char *)event->data.scalar.value);
            s->state = STATE_VKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_VSEEDLESS:
        switch (event->type) {
        case YAML_SCALAR_EVENT:
            if (get_boolean((char *)event->data.scalar.value, &s->v.seedless)) {
                fprintf(stderr, "Invalid boolean string value: %s\n",
                       (char *)event->data.scalar.value);
                return FAILURE;
            }
            s->state = STATE_VKEY;
            break;
        default:
            fprintf(stderr, "Unexpected event %d in state %d.\n", event->type, s->state);
            return FAILURE;
        }
        break;

    case STATE_STOP:
        break;
    }
    return SUCCESS;
}

int
main(int argc, char *argv[])
{
    int code;
    enum status status;
    struct parser_state state;
    yaml_parser_t parser;
    yaml_event_t event;

    if (getenv("DEBUG")) {
        debug = 1;
    }

    memset(&state, 0, sizeof(state));
    state.state = STATE_START;
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, stdin);
    do {
        status = yaml_parser_parse(&parser, &event);
        if (status == FAILURE) {
            fprintf(stderr, "yaml_parser_parse error\n");
            code = EXIT_FAILURE;
            goto done;
        }
        status = consume_event(&state, &event);
        if (status == FAILURE) {
            fprintf(stderr, "consume_event error\n");
            code = EXIT_FAILURE;
            goto done;
        }
        yaml_event_delete(&event);
    } while (state.state != STATE_STOP);

    /* Output the parsed data. */
    for (struct fruit *f = state.flist; f; f = f->next) {
        printf("fruit: name=%s, color=%s, count=%d\n", f->name, f->color, f->count);
        for (struct variety *v = f->varieties; v; v = v->next) {
            printf("  variety: name=%s, color=%s, seedless=%s\n", v->name, v->color, v->seedless ? "true" : "false");
        }
    }
    code = EXIT_SUCCESS;

done:
    free(state.f.name);
    free(state.f.color);
    free(state.v.name);
    free(state.v.color);
    destroy_fruits(&state.flist);
    destroy_varieties(&state.vlist);
    yaml_parser_delete(&parser);
    return code;
}
