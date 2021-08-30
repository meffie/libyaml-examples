/*
 * Example libyaml parser.
 *
 * This is a simple libyaml parser example which scans and prints
 * the libyaml parser events.
 *
 */
#include <yaml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDENT "  "
#define STRVAL(x) ((x) ? (char*)(x) : "")

void indent(int level)
{
    int i;
    for (i = 0; i < level; i++) {
        printf("%s", INDENT);
    }
}

void print_event(yaml_event_t *event)
{
    static int level = 0;

    switch (event->type) {
    case YAML_NO_EVENT:
        indent(level);
        printf("no-event (%d)\n", event->type);
        break;
    case YAML_STREAM_START_EVENT:
        indent(level++);
        printf("stream-start-event (%d)\n", event->type);
        break;
    case YAML_STREAM_END_EVENT:
        indent(--level);
        printf("stream-end-event (%d)\n", event->type);
        break;
    case YAML_DOCUMENT_START_EVENT:
        indent(level++);
        printf("document-start-event (%d)\n", event->type);
        break;
    case YAML_DOCUMENT_END_EVENT:
        indent(--level);
        printf("document-end-event (%d)\n", event->type);
        break;
    case YAML_ALIAS_EVENT:
        indent(level);
        printf("alias-event (%d)\n", event->type);
        break;
    case YAML_SCALAR_EVENT:
        indent(level);
        printf("scalar-event (%d) = {value=\"%s\", length=%d}\n",
               event->type,
               STRVAL(event->data.scalar.value),
               (int)event->data.scalar.length);
        break;
    case YAML_SEQUENCE_START_EVENT:
        indent(level++);
        printf("sequence-start-event (%d)\n", event->type);
        break;
    case YAML_SEQUENCE_END_EVENT:
        indent(--level);
        printf("sequence-end-event (%d)\n", event->type);
        break;
    case YAML_MAPPING_START_EVENT:
        indent(level++);
        printf("mapping-start-event (%d)\n", event->type);
        break;
    case YAML_MAPPING_END_EVENT:
        indent(--level);
        printf("mapping-end-event (%d)\n", event->type);
        break;
    }
    if (level < 0) {
        fprintf(stderr, "indentation underflow!\n");
        level = 0;
    }
}

int main(int argc, char *argv[])
{
    yaml_parser_t parser;
    yaml_event_t event;
    yaml_event_type_t event_type;

    yaml_parser_initialize(&parser);
    yaml_parser_set_input_file(&parser, stdin);

    do {
        if (!yaml_parser_parse(&parser, &event))
            goto error;
        print_event(&event);
        event_type = event.type;
        yaml_event_delete(&event);
    } while (event_type != YAML_STREAM_END_EVENT);

    yaml_parser_delete(&parser);
    return EXIT_SUCCESS;

error:
    fprintf(stderr, "Failed to parse: %s\n", parser.problem);
    yaml_parser_delete(&parser);
    return EXIT_FAILURE;
}
