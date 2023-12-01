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

/**
 * @file
 */

#ifndef __FANCY_MEMORY_H__
#define __FANCY_MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * @brief An opaque data type that is used as a container for storing "memory tracking"
 * related data, and which type gets passed, as a pointer argument, to the library's
 * methods for state interrogation and mutation.
 *
 * @note This object should always be created using the \ref fancy_memory_create()
 * factory method.
 *
 * @see fancy_memory_create
 *
 * @par Example:
 * @include examples/demo.c
 */
typedef struct fancy_memory_s fancy_memory_t;

/**
 * @brief A method that can be used to retrieve the library's current version. It works
 * by populating the arguments \p major , \p minor , and \p revision with
 * the library's major, minor, revision version numbers, respectively.
 *
 * @param major A pointer to a \ref uint16_t value to which the "major" version number will be written.
 * @param minor A pointer to a \ref uint16_t value to which the "minor" version number will be written.
 * @param revision A pointer to a \ref uint16_t value to which the "revision" version number will be written.
 */
void fancy_memory_get_library_version(uint16_t *major, uint16_t *minor, uint16_t *revision);

/**
 * @brief The factory method that needs to be used in order to instantiate a
 * \ref fancy_memory_t object.
 *
 * @return \ref fancy_memory_t* A pointer to the created \ref fancy_memory_t object.
 */
fancy_memory_t *fancy_memory_create(void);

/**
 * @brief The method that should be used to destroy a \ref fancy_memory_t
 * object once that object is no longer needed.
 *
 * @param self A pointer to the \ref fancy_memory_t instance to be destroyed.
 * @note Destroying the object will not free the tracked memory (i.e., the
 * memory that was allocated using the \ref fancy_memory_malloc and
 * \ref fancy_memory_realloc methods): it will only free the memory used
 * to track that memory (i.e., the memory contained inside the \ref fancy_memory_t
 * object).
 */
void fancy_memory_destroy(fancy_memory_t *self);

/**
 * @brief The method that must be used to allocate and track new memory.
 *
 * @param self A pointer to the \ref fancy_memory_t instance to be used to track
 * the new memory.
 * @param size The size of the memory to be allocated (and returned).
 * @return \ref void* A pointer to the newly allocated memory.
 * @note - Internally, this method makes a call to \ref malloc().
 * @note - If, internally, \ref malloc() returns the \ref NULL pointer, this method will
 * terminate the process.
 * @see fancy_memory_realloc, fancy_memory_free
 */
void *fancy_memory_malloc(fancy_memory_t *self, size_t size);

/**
 * @brief The method that must be used to free (and stop tracking) memory previously allocated
 * using the same \ref fancy_memory_t instance (i.e., the object pointed to by \p self ).
 *
 * @param self A pointer to the \ref fancy_memory_t instance that was initially used when
 * allocating the memory pointed to by \p pointer .
 * @param pointer A pointer to the memory to be freed, which was previously allocated
 * using the \ref fancy_memory_t instance pointed to by \p self .
 * @warning Passing a pointer to memory that was not initially allocated using \p self will result
 * in the process being terminated.
 * @note Internally, this method makes a call to \ref free().
 * @see fancy_memory_malloc, fancy_memory_realloc
 */
void fancy_memory_free(fancy_memory_t *self, void *pointer);

/**
 * @brief The method that must be used to reallocate (and update the tracking information of)
 * memory that was initially allocated using the same \ref fancy_memory_t
 * instance (i.e., the object pointed to by \p self ).
 *
 * @param self A pointer to the \ref fancy_memory_t instance that was used when
 * initially allocating the memory pointed to by \p pointer .
 * @param pointer A pointer to the memory to be reallocated, which was initially
 * allocated using the \ref fancy_memory_t instance pointed to by \p self .
 * @param size The size to be used for the re-allocation.
 * @return \ref void* A pointer to the re-allocated memory.
 * @warning Passing a pointer to memory that was not initially allocated using \p self will result
 * in the process being terminated.
 * @note - Internally, this method makes a call to \ref realloc().
 * @note - If, internally, \ref realloc() returns the \ref NULL pointer, this method will
 * terminate the process.
 * @see fancy_memory_malloc, fancy_memory_free
 */
void *fancy_memory_realloc(fancy_memory_t *self, void *pointer, size_t size);

/**
 * @brief A method that can be used to print (i.e., write) a summary of the tracked
 * memory for \p self .
 *
 * @param self A pointer to the \ref fancy_memory_t instance for which to print a summary.
 * @param stream A pointer to a writable stream (e.g., `stdout`, or a file pointer to a text
 * file) to which to print the summary.
 */
void fancy_memory_debug(fancy_memory_t const *self, FILE *stream);

/**
 * @brief The method that must be used to retrieve the total amount of memory (in bytes)
 * currently being tracked by the \ref fancy_memory_t object \p self .
 *
 * @param self A pointer to the \ref fancy_memory_t instance for which to retrieve
 * to total memory usage.
 * @return \ref size_t The total number of bytes currently being tracked (i.e., that
 * have been allocated but not yet freed) using \p self .
 */
size_t fancy_memory_get_total(fancy_memory_t const *self);

/**
 * @example examples/demo.c
 *
 * A simple, heavily annotated example that shows how all of the
 * library's API methods (i.e., functions) and types can be used.
 */

#endif
