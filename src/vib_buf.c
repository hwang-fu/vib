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

struct vib_buffer_t
{
    OWNED   char    * filename;     /* filename */
    OWNED   uint8_t * data;         /* memory-mapped data or NIL */
    COPIED  int       fd;           /* file descriptor */
    COPIED  uint64_t  size;         /* file size in bytes */
    COPIED  bool      is_mmap;      /* true if using mmap */
};

