//===-- load-ktest.cpp ------------------------------------------*- C++ -*-===//
//
// author: Quoc-Sang Phan<phanquocsang@gmail.com>
//
//===----------------------------------------------------------------------===//

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include "KTest.h"

#if defined(__FreeBSD__) || defined(__minix)
#define stat64 stat
#endif

static int read_uint32(FILE *f, unsigned *value_out) {
  unsigned char data[4];
  if (fread(data, 4, 1, f)!=1)
    return 0;
  *value_out = (((((data[0]<<8) + data[1])<<8) + data[2])<<8) + data[3];
  return 1;
}

static int read_string(FILE *f, char **value_out) {
  unsigned len;
  if (!read_uint32(f, &len))
    return 0;
  *value_out = (char*) malloc(len+1);
  if (!*value_out)
    return 0;
  if (fread(*value_out, len, 1, f)!=1)
    return 0;
  (*value_out)[len] = 0;
  return 1;
}

static void free_memory(KTest* res){
    unsigned i;
//  if (res) {
    if (res->args) {
      for (i=0; i<res->numArgs; i++)
        if (res->args[i])
          free(res->args[i]);
      free(res->args);
    }
    if (res->objects) {
      for (i=0; i<res->numObjects; i++) {
        KTestObject *bo = &res->objects[i];
        if (bo->name)
          free(bo->name);
        if (bo->bytes)
          free(bo->bytes);
      }
      free(res->objects);
    }
    free(res);
//  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <input-file> <output-file>\n", argv[0]);
    exit(1);
  }
  
  FILE *f = fopen(argv[1], "rb");
  KTest *res = 0;
  unsigned i, version;

  if (!f) 
    goto error;

  res = (KTest*) calloc(1, sizeof(*res));
  if (!res) 
    goto error;

  if (!read_uint32(f, &version)) 
    goto error;
  
  if (version > kTest_getCurrentVersion())
    goto error;

  res->version = version;

  if (!read_uint32(f, &res->numArgs)) 
    goto error;
  res->args = (char**) calloc(res->numArgs, sizeof(*res->args));
  if (!res->args) 
    goto error;
  
  for (i=0; i<res->numArgs; i++)
    if (!read_string(f, &res->args[i]))
      goto error;

  if (version >= 2) {
    if (!read_uint32(f, &res->symArgvs)) 
      goto error;
    if (!read_uint32(f, &res->symArgvLen)) 
      goto error;
  }

  if (!read_uint32(f, &res->numObjects))
    goto error;
  res->objects = (KTestObject*) calloc(res->numObjects, sizeof(*res->objects));
  if (!res->objects)
    goto error;
  for (i=0; i<res->numObjects; i++) {
    KTestObject *o = &res->objects[i];
    if (!read_string(f, &o->name))
      goto error;
    std::cout << std::endl << o->name << std::endl << std::flush;
    if (!read_uint32(f, &o->numBytes))
      goto error;
    o->bytes = (unsigned char*) malloc(o->numBytes);
    if (fread(o->bytes, o->numBytes, 1, f)!=1)
      goto error;
  }

  free_memory(res);
  fclose(f);

  exit(0);
 error:
  std::cout << std::endl << "Error parsing ktest file..." << std::endl << std::flush;
  if (f) free_memory(res);
  if (f) fclose(f);

  exit(1);
}
