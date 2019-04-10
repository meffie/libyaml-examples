/*
 * Example libyaml emitter.
 *
 * This is a basic example to demonstrate how to convert raw data to a yaml
 * stream using the libyaml emitter API. The example data to be converted is
 * is a simple array of structs,
 *
 *    struct fruit data[] = {
 *        {"apple", "red", 12},
 *        {"orange", "orange", 3},
 *        {"bannana", "yellow", 4},
 *        {"mango", "green", 1},
 *        {NULL, NULL, 0}
 *    };
 *
 * The exmaple data is converted into a yaml sequence of mapped values,
 *
 *    $ ./emit
 *    ---
 *    fruit:
 *    - name: apple
 *      color: red
 *      count: 12
 *    - name: orange
 *      color: orange
 *      count: 3
 *    - name: bannana
 *      color: yellow
 *      count: 4
 *    - name: mango
 *      color: green
 *      count: 1
 *    ...
 *
 * This example can be built and run on linux with the commands:
 *
 *     gcc -c emit.c -g -O0 -Wall
 *     gcc -o emit emit.o -lyaml
 *     ./emit
 *
 * See the libyaml project page http://pyyaml.org/wiki/LibYAML
 */
#include <yaml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fruit {
    char *name;
    char *color;
    int count;
};
struct fruit data[] = {
    {"apple", "red", 12},
    {"orange", "orange", 3},
    {"bannana", "yellow", 4},
    {"mango", "green", 1},
    {NULL, NULL, 0}
};

int main(int argc, char *argv[])
{
    yaml_emitter_t emitter;
    yaml_event_t event;
    struct fruit *f;
    char buffer[64];

    yaml_emitter_initialize(&emitter);
    yaml_emitter_set_output_file(&emitter, stdout);

    yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_document_start_event_initialize(&event, NULL, NULL, NULL, 0);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t *)YAML_MAP_TAG,
        1, YAML_ANY_MAPPING_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
        (yaml_char_t *)"fruit", strlen("fruit"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_sequence_start_event_initialize(&event, NULL, (yaml_char_t *)YAML_SEQ_TAG,
       1, YAML_ANY_SEQUENCE_STYLE);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    for (f = data; f->name; f++) {
        yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t *)YAML_MAP_TAG,
            1, YAML_ANY_MAPPING_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
            (yaml_char_t *)"name", strlen("name"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
            (yaml_char_t *)f->name, strlen(f->name), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
            (yaml_char_t *)"color", strlen("color"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
            (yaml_char_t *)f->color, strlen(f->color), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
            (yaml_char_t *)"count", strlen("count"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        sprintf(buffer, "%d", f->count);
        yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_INT_TAG,
            (yaml_char_t *)buffer, strlen(buffer), 1, 0, YAML_PLAIN_SCALAR_STYLE);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;

        yaml_mapping_end_event_initialize(&event);
        if (!yaml_emitter_emit(&emitter, &event)) goto error;
    }

    yaml_sequence_end_event_initialize(&event);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_mapping_end_event_initialize(&event);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_document_end_event_initialize(&event, 0);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_stream_end_event_initialize(&event);
    if (!yaml_emitter_emit(&emitter, &event)) goto error;

    yaml_emitter_delete(&emitter);
    return 0;

error:
    fprintf(stderr, "Failed to emit event %d: %s\n", event.type, emitter.problem);
    yaml_emitter_delete(&emitter);
    return 1;
}
