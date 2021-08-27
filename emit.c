/*
 * Example libyaml emitter.
 *
 * This is a basic example to demonstrate how to convert raw data to a yaml
 * stream using the libyaml emitter API. The example data to be converted is
 * a simple linked list of structs:
 *
 *   list -> {"apple", "red", 12} -> {"orange", "orange", 3} ->
 *           {"bannana", "yellow", 4} -> {"mango", "green", 1}
 *
 * The example data is converted into a yaml sequence of mapped values:
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
 * This example can be built and run on Linux with the commands:
 *
 *     $ sudo apt install libyaml-dev  # if Debian or Ubuntu
 *     $ make emit
 *     $ ./emit
 *
 * See the libyaml project page http://pyyaml.org/wiki/LibYAML
 */
#include <yaml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fruit.h"

int main(int argc, char *argv[])
{
    yaml_emitter_t emitter;
    yaml_event_t event;
    struct fruits list = {.head=NULL, .tail=NULL};

    /* Create our list of elements. */
    add_fruit(&list, "apple", "red", 12);
    add_fruit(&list, "orange", "orange", 3);
    add_fruit(&list, "bannana", "yellow", 4);
    add_fruit(&list, "mango", "green", 1);

    /* Emit list as yaml. */
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

    for (struct fruit *f = list.head; f; f = f->next) {
        char buffer[80];

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

        if (snprintf(buffer, sizeof(buffer), "%d", f->count) >= sizeof(buffer)) {
            bail("buffer truncation");
        }
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

    destroy_fruits(&list);
    return 0;

error:
    fprintf(stderr, "Failed to emit event %d: %s\n", event.type, emitter.problem);
    yaml_emitter_delete(&emitter);
    return 1;

    return 0;
}
