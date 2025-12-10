#include "vib_buf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include "memory.h"
#include "cstr.h"

/* ─────────────────────────────────────────────────────────────────────────────
 * Types
 * ───────────────────────────────────────────────────────────────────────────── */

struct vib_buffer_t
{
    OWNED   char    * filename;     /* filename */
    OWNED   uint8_t * data;         /* memory-mapped data or NIL */
    COPIED  int       fd;           /* file descriptor */
    COPIED  uint64_t  size;         /* file size in bytes */
    COPIED  bool      is_mmap;      /* true if using mmap */
};

/* ─────────────────────────────────────────────────────────────────────────────
 * Forward Declarations
 * ───────────────────────────────────────────────────────────────────────────── */

static COPIED result_t vib_buffer_mmap_(BORROWED vib_buffer_t * buf);

/* ─────────────────────────────────────────────────────────────────────────────
 * Lifecycle
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED result_t vib_buffer_open(BORROWED const char * path)
{
    if (!path)
    {
        return RESULT_ERR(VIB_BUF_ERR_NULL_PATH);
    }

    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        return RESULT_ERR(VIB_BUF_ERR_OPEN_FAILED);
    }

    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        close(fd);
        return RESULT_ERR(VIB_BUF_ERR_STAT_FAILED);
    }

    if (!S_ISREG(st.st_mode))
    {
        close(fd);
        return RESULT_ERR(VIB_BUF_ERR_NOT_REGULAR);
    }

    if (st.st_size == 0)
    {
        close(fd);
        return RESULT_ERR(VIB_BUF_ERR_EMPTY_FILE);
    }

    OWNED vib_buffer_t * buf = new(sizeof(vib_buffer_t));
    buf->filename = strdup_smart(path);
    buf->fd       = fd;
    buf->data     = NIL;
    buf->size     = CAST(st.st_size, uint64_t);
    buf->is_mmap  = false;

    if (RESULT_IS_ERR(vib_buffer_mmap_(buf)))
    {
        vib_buffer_close(buf);
        return RESULT_ERR(VIB_BUF_ERR_MMAP_FAILED);
    }

    return RESULT_OK(buf);
}

COPIED result_t vib_buffer_open_owned(OWNED const char * path)
{
    COPIED result_t result = vib_buffer_open(path);
    free_smart(path);
    return result;
}

void vib_buffer_close(OWNED vib_buffer_t * buf)
{
    if (!buf)
    {
        return;
    }

    if (buf->is_mmap && buf->data)
    {
        munmap(buf->data, buf->size);
        buf->data = NIL;
    }

    if (buf->fd >= 0)
    {
        close(buf->fd);
        buf->fd = -1;
    }

    free_smart(buf->filename);
    free_smart(buf);
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Memory Mapping
 * ───────────────────────────────────────────────────────────────────────────── */

static COPIED result_t vib_buffer_mmap_(BORROWED vib_buffer_t * buf)
{
    if (!buf || buf->fd < 0 || buf->size == 0)
    {
        return RESULT_ERR(1);
    }

    void * data = mmap(nil, buf->size, PROT_READ, MAP_PRIVATE, buf->fd, 0);
    if (data == MAP_FAILED)
    {
        return RESULT_ERR(2);
    }

    posix_madvise(data, buf->size, POSIX_MADV_SEQUENTIAL);

    buf->data    = CAST(data, uint8_t*);
    buf->is_mmap = true;

    return RESULT_OK(0);
}


/* ─────────────────────────────────────────────────────────────────────────────
 * File Info
 * ───────────────────────────────────────────────────────────────────────────── */

OWNED char * vib_buffer_get_path(BORROWED vib_buffer_t * buf)
{
    return buf ? strdup_smart(buf->filename) : strdup_smart("");
}

COPIED uint64_t vib_buffer_get_size(BORROWED vib_buffer_t * buf)
{
    return buf ? buf->size : 0;
}

COPIED result_t vib_buffer_try_get_path(BORROWED vib_buffer_t * buf)
{
    return buf ? RESULT_OK(strdup_smart(buf->filename)) : RESULT_ERR(1);
}

COPIED result_t vib_buffer_try_get_size(BORROWED vib_buffer_t * buf)
{
    return buf ? RESULT_OK(buf->size) : RESULT_ERR(1);
}

COPIED bool vib_buffer_is_mmap(BORROWED vib_buffer_t * buf)
{
    return buf ? buf->is_mmap : false;
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Data Access
 * ───────────────────────────────────────────────────────────────────────────── */

COPIED int32_t vib_buffer_byte_at(BORROWED vib_buffer_t * buf, COPIED const uint64_t offset)
{
}

OWNED uint8_t * vib_buffer_read_bytes(
        BORROWED vib_buffer_t * buf,
        COPIED uint64_t offset,
        COPIED uint64_t bytesExpected,
        BORROWED uint64_t * bytesActual)
{
}

/* ─────────────────────────────────────────────────────────────────────────────
 * Error Handling
 * ───────────────────────────────────────────────────────────────────────────── */

BORROWED const char * vib_buffer_strerror(COPIED const vib_buffer_err_t err)
{
    switch (err)
    {
        case VIB_BUF_OK:                return "Success";
        case VIB_BUF_ERR_NULL_PATH:     return "Path is NULL";
        case VIB_BUF_ERR_OPEN_FAILED:   return "Failed to open file";
        case VIB_BUF_ERR_STAT_FAILED:   return "Failed to stat file";
        case VIB_BUF_ERR_NOT_REGULAR:   return "Not a regular file";
        case VIB_BUF_ERR_MMAP_FAILED:   return "Failed to memory map file";
        case VIB_BUF_ERR_ALLOC_FAILED:  return "Memory allocation failed";
        case VIB_BUF_ERR_EMPTY_FILE:    return "File is empty";
        default:                        return "Unknown error";
    }
}


