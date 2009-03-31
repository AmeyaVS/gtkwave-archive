#ifndef LIBLZMA_H
#define LIBLZMA_H

#include <LzmaEnc.h>
#include <LzmaDec.h>

void *LZMA_fdopen(int fd, const char *mode);
void LZMA_close(void *handle);
size_t LZMA_flush(void *handle);
size_t LZMA_write(void *handle, void *mem, size_t len);
size_t LZMA_read(void *handle, void *mem, size_t len);

#endif
