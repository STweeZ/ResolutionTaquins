#pragma once

#include <stdbool.h>
#include <sys/types.h>

#define BUFFER_CAPACITY (8 * 10024)
#define BUFFER_SIZE 65536

typedef struct _dataBufferReader {
  int fd;
  unsigned pos, size, posBit;
  char *buffer;
} dataBufferReader;

dataBufferReader *createBufferReader(char *fileName);
void destroyBufferReader(dataBufferReader *data);
int getCurrentChar(dataBufferReader *data);
void consumeChar(dataBufferReader *data);
bool eof(dataBufferReader *data);
typedef struct _dataBufferWriter {
  int fd;
  unsigned pos;
  char *buffer;
} dataBufferWriter;

dataBufferWriter *createBufferWriter(char *fileName);
void destroyBufferWriter(dataBufferWriter *data);
void flush(dataBufferWriter *data);
void putBit(dataBufferWriter *data, bool val);
void putUnsignedChar(dataBufferWriter *data, u_int8_t val);