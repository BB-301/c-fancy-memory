# lib<fancy_memory>

**A very simple, very experimental C library for tracking dynamically allocated memory.**

## Introduction

`lib<fancy_memory>` is a very simple, very experimental C library for tracking dynamically allocated memory during C application and library development.

### Context

I wrote this library as part of another experimental C library project on which I am currently also working. Basically, that other project is making extensive use of dynamic memory allocation, and, while developing that library, I needed a quick and easy way of monitoring memory usage for the purpose of detecting memory leaks, so, instead of looking for an existing, well tested solution (what's the fun in that? LOL), I came up with the current solution, and later decided to publish it as its own standalone library.

## A quick example

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "fancy_memory.h"

#define MY_MESSAGE "hello"
#define MY_MESSAGE_SUFFIX " world"

int main(void)
{
    // We first start by checking which library version we are using.
    uint16_t major, minor, revision;
    fancy_memory_get_library_version(&major, &minor, &revision);
    // And we print that version to `stdout`.
    fprintf(
        stdout,
        "Current lib<fancy_memory> library version: %d.%d.%d\n",
        major,
        minor,
        revision);

    // We instantiate the memory tracker object.
    fancy_memory_t *memory_tracker = fancy_memory_create();
    // At this point, the tracker is empty (i.e., no allocation
    // has been made yet).
    assert(fancy_memory_get_total(memory_tracker) == 0);

    // We allocate (and track) memory to be able copy `MY_MESSAGE` into
    // a string. We store the allocated memory's address in a variable
    // named `my_string`.
    char *my_string = fancy_memory_malloc(memory_tracker, sizeof(char) * (strlen(MY_MESSAGE) + 1));
    // We populate, using `strcpy`, the newly allocated memory with `MY_MESSAGE`.
    strcpy(my_string, MY_MESSAGE);
    // We print that string to `stdout`.
    fprintf(stdout, "my_string = %s\n", my_string);

    // We use the "debug" library method to print a summary of the tracked
    // memory to `stdout`.
    fancy_memory_debug(memory_tracker, stdout);
    // We can also confirm that we are currently tracking `strlen(MY_MESSAGE) + 1`
    // bytes in total.
    assert(fancy_memory_get_total(memory_tracker) == (strlen(MY_MESSAGE) + 1));

    // We make a second, 4-byte allocation.
    uint8_t *my_bytes = fancy_memory_malloc(memory_tracker, sizeof(uint8_t) * 4);
    // We confirm that our total (tracked) memory usage has grown by 4 bytes.
    assert(fancy_memory_get_total(memory_tracker) == (strlen(MY_MESSAGE) + 1 + 4));

    // We can now look at the summary to see that it has been updated with more
    // information.
    fancy_memory_debug(memory_tracker, stdout);

    // We get rid of the memory pointed to by `my_bytes`.
    fancy_memory_free(memory_tracker, my_bytes);
    // We confirm that we are back at 4 bytes less.
    assert(fancy_memory_get_total(memory_tracker) == (strlen(MY_MESSAGE) + 1));

    // We want to append the string value contained in the
    // `MY_MESSAGE_SUFFIX` macro at the end of the string
    // pointed to by `my_string`, so we make a reallocation.
    my_string = fancy_memory_realloc(
        memory_tracker, my_string,
        sizeof(char) * (strlen(MY_MESSAGE) + strlen(MY_MESSAGE_SUFFIX) + 1));
    // And we use `memcpy` to populate the suffix bytes.
    memcpy((my_string + strlen(MY_MESSAGE)), MY_MESSAGE_SUFFIX, strlen(MY_MESSAGE_SUFFIX));
    my_string[strlen(MY_MESSAGE) + strlen(MY_MESSAGE_SUFFIX) + 1] = '\0';
    // We print the updated string to `stdout`.
    fprintf(stdout, "my_string = %s\n", my_string);

    // We confirm that our total (tracked) memory usage has grown by strlen(MY_MESSAGE_SUFFIX) bytes.
    assert(fancy_memory_get_total(memory_tracker) == (strlen(MY_MESSAGE) + strlen(MY_MESSAGE_SUFFIX) + 1));
    // And we look at the summary to see that it has been updated, again...
    fancy_memory_debug(memory_tracker, stdout);

    // We free the memory pointed to by `my_string`.
    fancy_memory_free(memory_tracker, my_string);
    // At this point, the tracker is empty again.
    assert(fancy_memory_get_total(memory_tracker) == 0);
    // And we look at the summary one last time.
    fancy_memory_debug(memory_tracker, stdout);

    // We have reached the end of the program, so we destroy
    // the memory tracker object. NOTE: In most cases, this won't
    // be necessary, but we do it here to have a complete example
    // where all of the library's methods and types are used.
    fancy_memory_destroy(memory_tracker);

    // We terminate the process.
    exit(EXIT_SUCCESS);
}
```

## Documentation

The library's API is documented using the [Doxygen](https://www.doxygen.nl/index.html) format (see [fancy_memory.h](include/fancy_memory.h)), such that it (the library) will play nicely with IDEs supporting that code documentation format, but, for convenience, this repository also has its own [GitHub-hosted Doxygen website](https://bb-301.github.io/c-fancy-memory-docs/v0.1.0/html/index.html), which should be consulted for a good overview of the library's API.

If you're interested, you may also clone this repository and generate the website yourself using the `make doxygen` recipe, which, however, assumes that you have [Docker](https://www.docker.com/) (or Docker Desktop) installed and running on your system. If you already have Doxygen installed on your system, you may modify the [Makefile](./Makefile) to use it instead of Docker. But please note that I used Doxygen version `1.9.6` while developing.

Finally, you can also have a look at the [examples](./examples) directory for quick examples of how this library can be used. As noted above, most of those examples are also embedded directly into the API documentation.

## Files and directories explained

* [doxygen](./doxygen) — A directory that contains files used to generate the Doxygen documentation website.
  * [Dockerfile](./doxygen/Dockerfile) — A `Dockerfile` that can be used to build an image containing Doxygen, which image can then be run as a container to generate the documentation website for this project.
  * [.dockerignore](./doxygen/.dockerignore) — A `.dockerignore` file used to limit the context used by Docker when building the image.
  * [Doxyfile](./doxygen/Doxyfile) — The [Doxygen configuration file](https://www.doxygen.nl/manual/config.html) used to generate the documentation website.
  * [main-page.md](./doxygen/main-page.md) — A markdown file that contains the Doxygen website's main page, which is used when building the website.
  * [libc-tag.xml](./doxygen/libc-tag.xml) — A custom made Doxygen `tagfile`, which corresponds to an XML file containing information about C types not defined by the (this) library, but for which I still wanted to generate hypertext links in the API documentation.
  * [c-keywords-tag.xml](./doxygen/c-keywords-tag.xml) — A custom made Doxygen `tagfile`, which corresponds to an XML file containing information about C types not defined by the (this) library, but for which I still wanted to generate hypertext links in the API documentation.

* [examples](./examples) — A directory that contains example files, which are referenced by the Doxygen website, but which can also be used as standalone example file.
  * [demo.c](./examples/demo.c) —  A simple, heavily annotated example that shows how all of the library's API methods (i.e., functions) and types can be used.

* [test](./test) —  A directory containing test files (unit and integration).
  * [main.c](./test/main.c) —  A simple file that it used to unit-test the individual library methods, while also performing an integration test in which we loop and periodically pause to allow visualizing memory usage (i.e., to check for potential leaks) using external tools. To run the test without the loop, the preprocessor flag `RUN_INTEGRATION_TEST = 0` should be set. If not specified, the flag will be defaulted to `RUN_INTEGRATION_TEST = 1`, which means that the "loop version" will be run. Note that the [Makefile](./Makefile) has two recipes for that: `make test_run_unit` and `make test_run_integration`, respectively.

* [include/fancy_memory.h](./include/fancy_memory.h) — The library's main (sole) header file, whose declarations are documented using the Doxygen code documentation format.

* [src/fancy_memory.c](./src/fancy_memory.c) —  The implementation file where all of the function and types declared in [fancy_memory.h](./include/fancy_memory.h) are defined.

* [Makefile](./Makefile) — A simple `Makefile` (for use with [GNU Make](https://www.gnu.org/software/make/)), which allows performing a few interesting tasks.

* [LICENSE](./LICENSE) — A file containing the copyright and licensing information for this project.

* [VERSION](./VERSION) — A simple text file containing the library's current version, using the `major.minor.revision` versioning format. The content of that file is used for automation purposes by [Makefile](./Makefile).

## Disclaimer

This is an experimental library. This library is not intended to be used in production: **the main use case for it is for detecting memory leaks during the development of C applications or libraries that are making extensive use of dynamic memory allocation**. Furthermore, this library's implementation has not be optimized. For instance, among other things, the library could potentially be made more efficient by limiting the number of internal calls made to `malloc` and `realloc`.

## FAQ

### Why the name `fancy_memory`?

Let me start by saying that there is nothing really "fancy" about this library. The main reason for the name is that I always try to namespace my C functions using prefixes that have very low probability of conflicting with function names from other libraries, so, in this case, I am pretty confident that not a lot of programmers will use the term `fancy_{something}` as prefix for their custom C types.

Another reason is that, to me, `fan` is a meaningful acronym, so I thought that it would be clever to combine that acronym with `C` (the programming language) to form `fan-C`, which sounds like `fancy` when pronouncing it.

### How does the library deal with `malloc` and `realloc` failing

At any point, if an internal call to `malloc` or `realloc` fails, the library will immediately terminate the process with an error message printed to `stderr`. Furthermore, the library will also terminate the process if any of the methods are called on an invalid `fancy_memory_t` instance pointer (e.g., if passing the `NULL` pointer to a method). Finally, the library will also terminate the process if calling `fancy_memory_realloc` or `fancy_memory_free` on a pointer to a memory address that is not (or no longer) being tracked by the `fancy_memory_t` instance.

### Why are you using the term "method" in the documentation?

This library's API uses a style some might call **C object-based programming**, in which, in order to mimic a class, an abstract data type is declared (in this case [fancy_memory_t](include/fancy_memory.h#L48)) that is used as a container for storing the object's data (note that, by using an abstract data type, the "class members" are made (somewhat) private, as opposed to what would be obtained when both declaring and defining the structure inside the header file). That object is then passed as a pointer to the library's public API functions, which are referred to here as "methods".

For instance, we create the main object like this:
```c
fancy_memory_t *memory_tracker = fancy_memory_create();
```
where `fancy_memory_create` corresponds to the "factory method" used to instantiate an object, whose pointer gets stored in the variable named `memory_tracker`. We then use that object to perform operations like the following:
```c
char* my_string = fancy_memory_malloc(memory_tracker, sizeof(char) * 10);
```
operation in which we allocate 10 "tracked" bytes and assign the returned pointer to a variable named `my_string`, which is declared as a `char` pointer. The `fancy_memory_malloc` function is referred to here as an "instance method" (or simply a "method"), because it opperates on an object. At any point inside the application's logic, we can check how much memory is being used as follows (i.e., using the `fancy_memory_get_total` method):
```c
assert(fancy_memory_get_total(memory_tracker) == 10);
```
Once we are done with using the memory pointed to by `my_string`, we free it as follows:
```c
fancy_memory_free(memory_tracker, my_string);
```
where `fancy_memory_free` is another function to which we refer here as an "instance method".

### How to use this library in my own C project?

The recommended approach is to use a [Git Submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

However, if you are not using Git for your project, the alternative is to copy and add both the [fancy_memory.h](include/fancy_memory.h) header file and the [fancy_memory.c](src/fancy_memory.c) implementation file, as well as the [LICENCE](LICENSE) information, directly to your project.

### What is the other project to which you are referring in the introduction section?

That project is still under development, but I plan on publishing it on GitHub as soon as I have a first version ready. So I'll update this section with more information about it before long.

## Still to do (maybe)...

* The current implementation (see [fancy_memory.c](src/fancy_memory.c)) could undoubtedly (at least, potentially) be improved and made more efficient. For instance, currently, each time a new allocation is made, the library calls `realloc` to increase the internal lists by one item (it does the same when an item is freed; i.e., it calls `realloc` to decrease the memory allocation for one item). A potentially better approach would be to keep a memory buffer to avoid calling `realloc` on each operation.
* The current API is relatively simple and there is a reason for that: I needed something quick and I stopped as soon as I had enough for my use case. But I may extend it in a future version. Here are a few additional methods that could be added that may be useful:
  * `size_t fancy_memory_get_item_count(fancy_memory_t const *self)` — Would return the number of items being tracked.
  * `void *fancy_memory_get_pointer_at_index(fancy_memory_t const *self, size_t index)` — Would return a pointer to the tracked item at position "index" in the internal list. The `NULL` pointer would be returned for an "out-of-bounds" index value.
  * `bool fancy_memory_includes(fancy_memory_t const *self, void const *pointer)` — Would allow testing if a specific pointer is being tracked by the tracker instance.
  * `ssize_t fancy_memory_get_size(fancy_memory_t const *self, void const *pointer)` — Would allow obtaining the allocated memory size for the tracked pointer. The value `-1` would be returned if the pointer was not being tracked by the tracker instance.
  
## Contact

If you have any questions, if you find bugs, or if you have suggestions for this project, please feel free to contact me by opening an issue on the [repository](https://github.com/BB-301/c-fancy-memory/issues).

## Licence

This project is released under the [MIT License](./LICENSE).