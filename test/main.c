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
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "fancy_memory.h"

#ifndef RUN_INTEGRATION_TEST
#define RUN_INTEGRATION_TEST (1)
#endif
#define TOTAL_NUMBER_OF_ITERATIONS (100000)
#define SLEEP_DURATION_IN_SECONDS (1)
#define NUMBER_OF_ITERATIONS_BEFORE_SLEEP (1000)

#if (RUN_INTEGRATION_TEST == 1)
static void sleep_seconds(size_t n);
#endif

int main(void)
{
    fancy_memory_t *m = fancy_memory_create();
    assert(m != NULL);
    assert(fancy_memory_get_total(m) == 0);

#if (RUN_INTEGRATION_TEST == 1)
    for (size_t i = 0; i < TOTAL_NUMBER_OF_ITERATIONS; i++)
    {
        if (i > 0 && (i % NUMBER_OF_ITERATIONS_BEFORE_SLEEP) == 0)
        {
            fprintf(stdout, "[iter: %zu] Sleeping 1 seconds...\n", i);
            sleep_seconds(SLEEP_DURATION_IN_SECONDS);
        }
#endif
        char *bytes_1 = fancy_memory_malloc(m, 100);
        assert(fancy_memory_get_total(m) == 100);

        char *bytes_2 = fancy_memory_malloc(m, 200);
        assert(fancy_memory_get_total(m) == 300);

        bytes_2 = fancy_memory_realloc(m, bytes_2, 300);
        assert(fancy_memory_get_total(m) == 400);

        bytes_1 = fancy_memory_realloc(m, bytes_1, 50);
        assert(fancy_memory_get_total(m) == 350);

        fancy_memory_free(m, bytes_2);
        assert(fancy_memory_get_total(m) == 50);

        fancy_memory_free(m, bytes_1);
        assert(fancy_memory_get_total(m) == 0);
#if (RUN_INTEGRATION_TEST == 1)
    }
#endif

    fancy_memory_destroy(m);
}

#if (RUN_INTEGRATION_TEST == 1)
static void sleep_seconds(size_t n)
{
#ifdef _WIN32
    Sleep(n * 1000);
#else
    sleep(n);
#endif
}
#endif
