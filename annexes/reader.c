#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "../annexes/reader.h"

dataBufferReader *createBufferReader(char *fileName) {
  dataBufferReader *ret = (dataBufferReader *)malloc(sizeof(dataBufferReader));
  ret->fd = open(fileName, O_RDONLY);
  if (ret->fd == -1) {
    perror("Cannot open: ");
    free(ret);
    exit(1);
  }

  ret->buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
  if (!ret) {
    fprintf(stderr, "Error when allocating memory\n");
    free(ret);
    exit(2);
  }

  ret->size = read(ret->fd, ret->buffer, BUFFER_SIZE);
  if (ret->size == -1) {
    perror("Cannot read: ");
    free(ret->buffer);
    free(ret);
    exit(3);
  }

  ret->posBit = 0;
  ret->pos = 0;
  return ret;
}

void destroyBufferReader(dataBufferReader *data) {
  if (close(data->fd) == -1) {
    perror("Cannot close\n");
    exit(1);
  }
  free(data->buffer);
  free(data);
}

int getCurrentChar(dataBufferReader *data) {
  if (eof(data))
    return -1;
  return data->buffer[data->pos];
}

void consumeChar(dataBufferReader *data) {
  data->pos++;
  if (data->pos >= data->size) {
    data->size = read(data->fd, data->buffer, BUFFER_SIZE);
    if (data->size == -1) {
      perror("Cannot read\n");
      exit(1);
    }
    data->pos = 0;
  }
  data->posBit = 0;
}

bool eof(dataBufferReader *data) { return !data->size; }

dataBufferWriter *createBufferWriter(char *fileName) {
    dataBufferWriter *data = malloc(sizeof(dataBufferWriter));
    data->fd = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if(data->fd == -1) {
        perror("Failed to open :");
        free(data);
        exit(1);
    }
    data->buffer = malloc(BUFFER_CAPACITY * sizeof(char));
    if(!data) {
        printf("Error when allocating memory\n");
        close(data->fd);
        free(data);
        return NULL;
    }
    data->pos = 0;
    return data;
}

void destroyBufferWriter(dataBufferWriter *data) {
    flush(data);
    if(close(data->fd) == -1) {
        perror("Failed to close :");
        exit(1);
    }
    free(data->buffer);
    free(data);
}

void flush(dataBufferWriter *data) {
  int i, cpt = 0;
  u_int8_t tmp = 0;
  for (i = 0; i < data->pos; i++) {
    if (cpt == 8) {
      write(data->fd, &tmp, sizeof(tmp));
      cpt = 0;
      tmp = 0;
    }

    cpt++;
    tmp |= (data->buffer[i] & 1) << (8 - cpt);
  }

  if (cpt)
    write(data->fd, &tmp, sizeof(tmp));
}

void putBit(dataBufferWriter *data, bool val) {
  assert(data->pos < BUFFER_CAPACITY);
  data->buffer[data->pos++] = val & 1;
  if (data->pos == BUFFER_CAPACITY) {
    flush(data);
    data->pos = 0;
  }
}

void putUnsignedChar(dataBufferWriter *data, u_int8_t val) {
  unsigned i = 0, shift = (sizeof(val) * 8) - 1;
  for (i = 0; i < (sizeof(val) * 8); i++) {
    putBit(data, (val >> shift) & 1);
    val <<= 1;
  }
}