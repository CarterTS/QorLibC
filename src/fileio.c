#include "stdio.h"

#include "errno.h"
#include "stdlib.h"
#include "string.h"
#include "sys/syscalls.h"

int fclose(FILE* f)
{
    int result = sys_close(f->fd);

    if (result < 0)
    {
        errno = -result;
        return 4;
    }

    if (f != stderr && f != stdout && f != stdin)
    {
        free((void*)f);
    }
    
    return 0;
}

FILE* fopen(const char* name, const char* mode)
{
    size_t raw_mode;

    if (strcmp(mode, "w") == 0 || strcmp(mode, "wb") == 0)
    {
        raw_mode = O_WRONLY | O_CREAT | O_TRUNC;
    }
    else if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0)
    {
        raw_mode = O_RDONLY;
    }
    else if (strcmp(mode, "w+") == 0 || strcmp(mode, "wb+") == 0 || strcmp(mode, "w+b") == 0 ||
             strcmp(mode, "r+") == 0 || strcmp(mode, "rb+") == 0 || strcmp(mode, "r+b") == 0)
    {
        raw_mode = O_RDWR | O_CREAT;
    }
    else if (strcmp(mode, "a") == 0 || strcmp(mode, "ab") == 0)
    {
        raw_mode = O_RDWR | O_APPEND;
    }
    else if (strcmp(mode, "a+") == 0 || strcmp(mode, "ab+") == 0 || strcmp(mode, "a+b") == 0)
    {
        raw_mode = O_RDWR | O_APPEND | O_CREAT;
    }
    else
    {
        errno = EINVAL;
        return 0;
    }

    bool is_binary = false;

    for (int i = 0; mode[i]; i++)
    {
        if (mode[i] == 'b')
        {
            is_binary = true;
            break;
        }
    }

    int result = sys_open(name, raw_mode);

    if (result < 0)
    {
        errno = -result;
        return 0;
    }

    FILE* f = malloc(sizeof(FILE));
    f->fd = result;
    f->is_binary = is_binary;

    return f;
}

size_t fread(void* ptr, size_t size, size_t nitems, FILE* stream)
{
    int result = sys_read(stream->fd, ptr, size * nitems);

    if (result < 0)
    {
        errno = -result;
        return 0;
    }

    return result / size;
}


size_t fwrite(const void* ptr, size_t size, size_t nitems, FILE * stream)
{
    int result = sys_write(stream->fd, (void*)ptr, size * nitems);

    if (result < 0)
    {
        errno = -result;
        return 0;
    }

    return result / size;
}


int fseek(FILE* stream, long offset, int origin)
{
    int result = sys_lseek(stream->fd, offset, origin);

    if (result < 0)
    {
        errno = -result;
        return 0;
    }

    return result;
}

int puts(const char* str)
{
    int result = fwrite(str, strlen(str), 1, stdin);
    
    if (result < 0)
    {
        return result;
    }
    
    result = fwrite("\n", 1, 1, stdin);
    return result;
}