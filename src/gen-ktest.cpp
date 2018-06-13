//===-- gen-ktest.cpp -------------------------------------*- C++ -*-===//
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

#define MAX 64
#define MAX_SIZE 30

static void push_int(KTest *b, const char *name, unsigned val) {
  KTestObject *o = &b->objects[b->numObjects++];
  assert(b->numObjects < MAX);
  // o->name = strdup(name);
  o->name = strdup(name);
  // integer in C++ is 4 bytes
  o->numBytes = 4;
  o->bytes = (unsigned char *)malloc(4);
  unsigned i;
  for (int i = 0; i < 4; i++)
         o->bytes[i] = (val << (i * 8));  
}

/*
User this option if the symbolic variables are of interger type, created using
the API klee_make_symbolic
*/
static void gen_from_vars(KTest *b, const char *input_file, const char *output_file){
  char sym_var[MAX_SIZE];
  unsigned int val;
  std::ifstream infile(input_file);
  while (infile >> sym_var >> val)
  {
    std::cout << sym_var << " " << val << std::endl;
    push_int(b, sym_var, val);
  }
  
  if (!kTest_toFile(b, output_file))
    assert(0);
}

/*
Use this option if the input file is symbolic
*/
static void gen_from_file(KTest *b, const char *input_file, const char *output_file){

}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <option> <input-file> <output-file>\n", argv[0]);
    fprintf(stderr, "each line of <input-file> has the form: variable value\n");
    exit(1);
  }
  
  KTest b;
  b.numArgs = argc;
  b.args = argv;
  b.symArgvs = 0;
  b.symArgvLen = 0;

  b.numObjects = 0;
  b.objects = (KTestObject *)malloc(MAX * sizeof *b.objects);
  
	if(strcmp(argv[1],"--sym-vars") == 0){
		gen_from_vars(&b, argv[2], argv[3]);		
	} else if(strcmp(argv[1], "--sym-file") == 0){
		gen_from_file();
	} else{
    fprintf(stderr, "Unknown option: %s\n", argv[1]);
	}
	
  return 0;
}
