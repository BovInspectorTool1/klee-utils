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

#define KTEST_MAGIC_SIZE 5

#if defined(__FreeBSD__) || defined(__minix)
#define stat64 stat
#endif

/// The PC is little indian
/// ktest file stores integers in big indian 
static unsigned int read_uint32(std::fstream *f){
  unsigned char data[4];
  f->read(reinterpret_cast<char*>(data), 4);
  return (((((data[0]<<8) + data[1])<<8) + data[2])<<8) + data[3];
}

/// The PC is little indian
/// ktest file stores integers in big indian 
static std::string read_string(std::fstream *f){
  unsigned int len = read_uint32(f);
  char data[len+1];
  f->read(data, len);
  data[len] = '\0';
  return std::string(data);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <input-file> <output-file>\n", argv[0]);
    exit(1);
  }
  
  std::fstream fs(argv[1], std::ios_base::in | std::ios_base::binary);
  
  // skip string type: KTEST or BOUT, both have 5 characters
  fs.seekg (KTEST_MAGIC_SIZE, std::ios_base::beg);
  
  // don't care about version, skip 4 bytes
  fs.seekg (4, std::ios_base::cur);
  
  // read number of arguments
  int numArgs = read_uint32(&fs);
  
  // skip all the arguments
  int i, len;
  for(i = 0; i < numArgs; ++i){
    // each string starts with its length. See function "kTest_toFile" in KTest.cpp
    len = read_uint32(&fs);
    // skip that string
    fs.seekg(len, std::ios_base::cur);
  }
  
  // skip symArgvs
  fs.seekg(4, std::ios_base::cur);
  
  // skip symArgvLen
  fs.seekg(4, std::ios_base::cur);
  
  // read numObjects to iterate
  int numObjects = read_uint32(&fs);
  unsigned int size;
  for (i=0; i< numObjects; i++) {
    std::string name = read_string(&fs);
    //*
    if (name.find("-data") == std::string::npos){
      size = read_uint32(&fs);
      fs.seekg(size, std::ios_base::cur);
      continue;
    }
    //*/
    std::cout << std::endl << "name = " << name << std::endl;
    size = read_uint32(&fs);
    std::cout << std::endl << "size = " << size << std::endl;
    char data[size+1];
    fs.read(data, size);
    data[size] = '\0';
    std::ofstream ofs(argv[2], std::ios::binary);
    ofs.write(data, size);
    ofs.close();
    std::cout << std::endl << "data = '" << std::flush;
    /*
    for (const char* p = data; *p; ++p)
    {
        printf("%04x", *p);
    }
    /*/
    std::cout << data << "'" << std::endl << std::flush;
    break;
  }
  
  fs.close();
}
