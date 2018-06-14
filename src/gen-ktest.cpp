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

static void push_int(KTest *b, const char *name, int val) {
  KTestObject *o = &b->objects[b->numObjects++];
  assert(b->numObjects < MAX);
  // o->name = strdup(name);
  o->name = strdup(name);
  // integer in C++ is 4 bytes
  o->numBytes = 4;
  o->bytes = (unsigned char *)malloc(4);
	//FIXME: the bytes are not displayed correctly in ktest-tool for value > 40
	o->bytes[0] = val & 0xFF;
	o->bytes[1] = (val >> 8) & 0xFF;
  o->bytes[2] = (val >> 16) & 0xFF;
  o->bytes[3] = (val >> 24) & 0xFF;
}

static void push_obj(KTest *b, const char *name, unsigned non_zero_bytes, 
                     unsigned total_bytes) {
  KTestObject *o = &b->objects[b->numObjects++];
  assert(b->numObjects < MAX);

  o->name = strdup(name);
  o->numBytes = total_bytes;
  o->bytes = (unsigned char *)malloc(o->numBytes);

  unsigned i;
  for(i = 0; i < non_zero_bytes; i++)
    o->bytes[i] = random();

  for(i = non_zero_bytes; i < total_bytes; i++)
    o->bytes[i] = 0;
}

/*
User this option if the symbolic variables are of interger type, created using
the API klee_make_symbolic
*/
static void gen_from_vars(KTest *b, const char *input_file){
  char sym_var[MAX_SIZE];
  int val;
  std::ifstream infile(input_file);
  while (infile >> sym_var >> val)
  {
    std::cout << sym_var << " " << val << std::endl;
    push_int(b, sym_var, val);
  }
}

/*
Use this option if the input file is symbolic
*/
static void gen_from_file(KTest *b, const char *input_file){
 	std::ifstream inFile; 
	size_t size = 0;
	inFile.open(input_file, std::ios::in|std::ios::binary|std::ios::ate);
	
	if(inFile.is_open()) {

		inFile.seekg(0, std::ios::end);
		size = inFile.tellg();
		inFile.seekg(0, std::ios::beg);

		KTestObject *o = &b->objects[b->numObjects++];
	  assert(b->numObjects < MAX);
	  o->name = strdup("file");
	  o->numBytes = size + 1;
	  o->bytes = (unsigned char *)malloc(size + 1);

		// FIXME: is it a good type cast?
		inFile.read((char*)o->bytes, size);

		// Does it need to be terminated by '\0'
		o->bytes[size] = '\0';
		// FIXME: Not sure if this is necessary? but KLEE used in generating random seed
		push_obj(b, "file-stat", sizeof(struct stat64), sizeof(struct stat64));
	}
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
		gen_from_vars(&b, argv[2]);		
	} else if(strcmp(argv[1], "--sym-file") == 0){
		gen_from_file(&b, argv[2]);
	} else{
    fprintf(stderr, "Unexpected option: <%s>\n", argv[1]);
	}

  if (!kTest_toFile(&b, argv[3]))
    assert(0);	

  return 0;
}
