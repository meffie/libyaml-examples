# libyaml examples

This is a small set of c language example programs to demonstrate how to use
the [libyaml library](http://pyyaml.org/wiki/LibYAML).

## Emitter example

`emit.c` is a basic example to demonstrate how to convert raw c structs to a
yaml stream using the libyaml emitter API.  The example data to be converted is
is a simple array of structs:

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

The example data is converted into a yaml sequence of mapped values,

    $ ./emit
    ---
    fruit:
    - name: apple
      color: red
      count: 12
    - name: orange
      color: orange
      count: 3
    - name: bannana
      color: yellow
      count: 4
    - name: mango
      color: green
      count: 1
    ...

## Parser example

`parser.c` is a basic example to demonstrate how to convert yaml to raw data
using the libyaml emitter API. Example yaml data to be parsed:

    $ cat fruit.yaml
    ---
    fruit:
    - name: apple
    color: red
    count: 12
    - name: orange
    color: orange
    count: 3
    - name: bannana
    color: yellow
    count: 4
    - name: mango
    color: green
    count: 1
    ...

    $ ./parse < fruit.yaml
    data[0]={name=apple, color=red, count=12}
    data[1]={name=orange, color=orange, count=3}
    data[2]={name=bannana, color=yellow, count=4}
    data[3]={name=mango, color=green, count=1}

## Scanner example

`scan.c` is a basic libyaml parser example which scans and prints
the libyaml parser events.

    $ ./scan < fruit.yaml | head
    stream-start-event
      document-start-event
        mapping-start-event
          scalar-event={value="fruit", length=5}
          sequence-start-event
            mapping-start-event
              scalar-event={value="name", length=4}
              scalar-event={value="apple", length=5}
              scalar-event={value="color", length=5}
              scalar-event={value="red", length=3}

