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

#include "fancy_memory.h"

#define __FANCY_MEMORY_LIB_VERSION_MAJOR__ 0
#define __FANCY_MEMORY_LIB_VERSION_MINOR__ 1
#define __FANCY_MEMORY_LIB_VERSION_REVISION__ 0

#define FAIL_AND_TERMINATE(message)                                          \
    fprintf(stderr, "[%s:%d] ERROR: %s\n", __func__, __LINE__, message); \
    exit(EXIT_FAILURE)

static size_t fancy_memory_private_member_next_index(fancy_memory_t *self);
static ssize_t fancy_memory_private_member_index_of(fancy_memory_t *self, void *pointer);
static void fancy_memory_private_member_remove(fancy_memory_t *self, size_t index);

struct fancy_memory_s
{
    size_t n;
    void **pointers;
    size_t *sizes;
};

void fancy_memory_get_library_version(uint16_t *major, uint16_t *minor, uint16_t *revision)
{
    *major = __FANCY_MEMORY_LIB_VERSION_MAJOR__;
    *minor = __FANCY_MEMORY_LIB_VERSION_MINOR__;
    *revision = __FANCY_MEMORY_LIB_VERSION_REVISION__;
}

fancy_memory_t *fancy_memory_create(void)
{
    fancy_memory_t *self = malloc(sizeof(fancy_memory_t));
    if (self == NULL)
    {
        FAIL_AND_TERMINATE("Call to 'malloc' returned the NULL pointer.");
    }
    self->n = 0;
    self->pointers = NULL;
    self->sizes = NULL;
    return self;
}

void fancy_memory_destroy(fancy_memory_t *self)
{
    if (self->sizes != NULL)
    {
        free(self->sizes);
        self->sizes = NULL;
    }
    if (self->pointers != NULL)
    {
        free(self->pointers);
        self->pointers = NULL;
    }
    free(self);
}

void *fancy_memory_malloc(fancy_memory_t *self, size_t size)
{
    void *pointer = malloc(size);
    if (pointer == NULL)
    {
        FAIL_AND_TERMINATE("Call to 'malloc' returned the NULL pointer.");
    }
    size_t next_index = fancy_memory_private_member_next_index(self);
    self->pointers[next_index] = pointer;
    self->sizes[next_index] = size;
    return pointer;
}

void fancy_memory_free(fancy_memory_t *self, void *pointer)
{
    ssize_t index = fancy_memory_private_member_index_of(self, pointer);
    if (index == -1)
    {
        FAIL_AND_TERMINATE("Trying to free memory for address that is not being tracked by the specified instance.");
    }
    fancy_memory_private_member_remove(self, (size_t)index);
}

void *fancy_memory_realloc(fancy_memory_t *self, void *pointer, size_t size)
{
    ssize_t index = fancy_memory_private_member_index_of(self, pointer);
    if (index == -1)
    {
        FAIL_AND_TERMINATE("Trying to reallocate memory for address that is not being tracked by the specified instance.");
    }
    self->pointers[index] = realloc(self->pointers[index], size);
    if (self->pointers[index] == NULL)
    {
        FAIL_AND_TERMINATE("Call to 'realloc' returned the NULL pointer.");
    }
    self->sizes[index] = size;
    return self->pointers[index];
}

void fancy_memory_debug(fancy_memory_t const *self, FILE *stream)
{
    if (self->n == 0)
    {
        fprintf(stream, "\nfancy_memory_t[%zu] {} [total size = 0]\n\n", self->n);
        return;
    }
    size_t total_size = 0;
    fprintf(stream, "\nfancy_memory_t[%zu] {\n", self->n);
    for (size_t i = 0; i < self->n; i++)
    {
        fprintf(stream, "\t.[%zu] = { address = %p, size = %zu },\n", i, self->pointers[i], self->sizes[i]);
        total_size += self->sizes[i];
    }
    fprintf(stream, "} [total size = %zu]\n\n", total_size);
}

size_t fancy_memory_get_total(fancy_memory_t const *self)
{
    size_t total_size = 0;
    for (size_t i = 0; i < self->n; i++)
    {
        total_size += self->sizes[i];
    }
    return total_size;
}

static size_t fancy_memory_private_member_next_index(fancy_memory_t *self)
{
    size_t index = self->n;
    self->n += 1;
    if (index == 0)
    {
        self->pointers = malloc(sizeof(void **));
        if (self->pointers == NULL)
        {
            FAIL_AND_TERMINATE("Call to 'malloc' returned the NULL pointer.");
        }
        self->sizes = malloc(sizeof(size_t));
        if (self->sizes == NULL)
        {
            FAIL_AND_TERMINATE("Call to 'malloc' returned the NULL pointer.");
        }
    }
    else
    {
        self->pointers = realloc(self->pointers, sizeof(void **) * self->n);
        if (self->pointers == NULL)
        {
            FAIL_AND_TERMINATE("Call to 'realloc' returned the NULL pointer.");
        }
        self->sizes = realloc(self->sizes, sizeof(size_t) * self->n);
        if (self->sizes == NULL)
        {
            FAIL_AND_TERMINATE("Call to 'realloc' returned the NULL pointer.");
        }
    }
    return index;
}

static ssize_t fancy_memory_private_member_index_of(fancy_memory_t *self, void *pointer)
{
    for (size_t i = 0; i < self->n; i++)
    {
        if (self->pointers[i] == pointer)
        {
            return i;
        }
    }
    return -1;
}

static void fancy_memory_private_member_remove(fancy_memory_t *self, size_t index)
{
    if (self->n == 1)
    {
        free(self->pointers[0]);
        free(self->pointers);
        free(self->sizes);
        self->pointers = NULL;
        self->sizes = NULL;
        self->n = 0;
        return;
    }

    free(self->pointers[index]);

    for (size_t i = index + 1; i < self->n; i++)
    {
        self->pointers[i - 1] = self->pointers[i];
        self->sizes[i - 1] = self->sizes[i];
    }

    self->n -= 1;
    self->pointers = realloc(self->pointers, sizeof(void **) * self->n);
    if (self->pointers == NULL)
    {
        FAIL_AND_TERMINATE("Call to 'realloc' returned the NULL pointer.");
    }
    self->sizes = realloc(self->sizes, sizeof(size_t) * self->n);
    if (self->sizes == NULL)
    {
        FAIL_AND_TERMINATE("Call to 'realloc' returned the NULL pointer.");
    }
}
