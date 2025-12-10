#pragma once

#include "common.h"
#include "result.h"

/*
 * vib_buffer — File buffer module
 *
 * Provides efficient file access via memory mapping (mmap).
 * Falls back to pread for special files.
 */

typedef enum vib_buffer_err_t
{
    VIB_BUF_OK = 0,
    VIB_BUF_ERR_NULL_PATH,
    VIB_BUF_ERR_OPEN_FAILED,
    VIB_BUF_ERR_STAT_FAILED,
    VIB_BUF_ERR_NOT_REGULAR,
    VIB_BUF_ERR_MMAP_FAILED,
    VIB_BUF_ERR_ALLOC_FAILED,
    VIB_BUF_ERR_EMPTY_FILE,
} vib_buffer_err_t;

typedef struct vib_buffer_t vib_buffer_t;

COPIED result_t vib_buffer_open(BORROWED const char * path);
COPIED result_t vib_buffer_open_owned(OWNED const char * path);
void vib_buffer_close(OWNED vib_buffer_t * buf);

OWNED char * vib_buffer_get_path(BORROWED vib_buffer_t * buf);
COPIED uint64_t vib_buffer_get_size(BORROWED vib_buffer_t * buf);

COPIED result_t vib_buffer_try_get_path(BORROWED vib_buffer_t * buf);
COPIED result_t vib_buffer_try_get_size(BORROWED vib_buffer_t * buf);

/* whether the buffer is using mmap or not */
COPIED bool vib_buffer_is_mmap(BORROWED vib_buffer_t * buf);


COPIED int32_t vib_buffer_byte_at(BORROWED vib_buffer_t * buf, COPIED const uint64_t offset);

OWNED uint8_t * vib_buffer_read_bytes(
        BORROWED vib_buffer_t * buf,
        COPIED uint64_t offset,
        COPIED uint64_t bytesExpected,
        BORROWED uint64_t * bytesActual);


BORROWED const char * vib_buffer_strerror(COPIED const vib_buffer_err_t err);

