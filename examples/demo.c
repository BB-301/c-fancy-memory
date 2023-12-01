/*
    Copyright (c) 2023 BB-301 <fw3dg3@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the “Software”), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

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
