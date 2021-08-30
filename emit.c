/*
 * Example libyaml emitter.
 *
 * This is a basic example to demonstrate how to convert raw data to a yaml
 * stream using the libyaml emitter API.
 *
 * Example output:
 *
 *    $ ./emit
 *    ---
 *    fruit:
 *    - name: apple
 *      color: red
 *      count: 12
 *      varieties:
 *      - name: macintosh
 *        color: red
 *        seedless: false
 *      - name: granny smith
 *        color: green
 *        seedless: false
 *      - name: red delicious
 *        color: red
 *        seedless: false
 *    - name: orange
 *      color: orange
 *      count: 3
 *      varieties:
 *      - name: naval
 *        color: orange
 *        seedless: false
 *      - name: clementine
 *        color: orange
 *        seedless: true
 *      - name: valencia
 *        color: orange
 *        seedless: false
 *    - name: bannana
 *      color: yellow
 *      count: 4
 *      varieties:
 *      - name: cavendish
 *        color: yellow
 *        seedless: true
 *      - name: plantain
 *        color: green
 *        seedless: true
 *    - name: mango
 *      color: green
 *      count: 1
 *      varieties:
 *      - name: honey
 *        color: yellow
 *        seedless: false
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
    struct fruit *fruits = NULL;
    struct variety *varieties = NULL;

    /* Create our list of lists. */
    varieties = NULL;
    add_variety(&varieties, "macintosh", "red", false);
    add_variety(&varieties, "granny smith", "green", false);
    add_variety(&varieties, "red delicious", "red", false);
    add_fruit(&fruits, "apple", "red", 12, varieties);

    varieties = NULL;
    add_variety(&varieties, "naval", "orange", false);
    add_variety(&varieties, "clementine", "orange", true);
    add_variety(&varieties, "valencia", "orange", false);
    add_fruit(&fruits, "orange", "orange", 3, varieties);

    varieties = NULL;
    add_variety(&varieties, "cavendish", "yellow", true);
    add_variety(&varieties, "plantain", "green", true);
    add_fruit(&fruits, "bannana", "yellow", 4, varieties);

    varieties = NULL;
    add_variety(&varieties, "honey", "yellow", false);
    add_fruit(&fruits, "mango", "green", 1, varieties);

    /* Emit list of lists as yaml. */
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

    for (struct fruit *f = fruits; f; f = f->next) {
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

        if (f->varieties) {
            yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
                (yaml_char_t *)"varieties", strlen("varieties"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
            if (!yaml_emitter_emit(&emitter, &event)) goto error;

            yaml_sequence_start_event_initialize(&event, NULL, (yaml_char_t *)YAML_SEQ_TAG,
                1, YAML_ANY_SEQUENCE_STYLE);
            if (!yaml_emitter_emit(&emitter, &event)) goto error;

            for (struct variety *v = f->varieties; v; v = v->next) {
                yaml_mapping_start_event_initialize(&event, NULL, (yaml_char_t *)YAML_MAP_TAG,
                    1, YAML_ANY_MAPPING_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
                    (yaml_char_t *)"name", strlen("name"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
                    (yaml_char_t *)v->name, strlen(v->name), 1, 0, YAML_PLAIN_SCALAR_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
                    (yaml_char_t *)"color", strlen("color"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
                    (yaml_char_t *)v->color, strlen(v->color), 1, 0, YAML_PLAIN_SCALAR_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_STR_TAG,
                    (yaml_char_t *)"seedless", strlen("seedless"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_scalar_event_initialize(&event, NULL, (yaml_char_t *)YAML_INT_TAG,
                    (yaml_char_t *)(v->seedless ? "true" : "false"),
                    strlen(v->seedless ? "true" : "false"), 1, 0, YAML_PLAIN_SCALAR_STYLE);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;

                yaml_mapping_end_event_initialize(&event);
                if (!yaml_emitter_emit(&emitter, &event)) goto error;
            }
            yaml_sequence_end_event_initialize(&event);
            if (!yaml_emitter_emit(&emitter, &event)) goto error;
        }

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
    destroy_fruits(&fruits);
    return EXIT_SUCCESS;

error:
    fprintf(stderr, "Failed to emit event %d: %s\n", event.type, emitter.problem);
    yaml_emitter_delete(&emitter);
    destroy_fruits(&fruits);
    return EXIT_FAILURE;
}
