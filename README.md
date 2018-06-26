# klee-utils
Some utils for the KLEE symbolic execution engine

gen-ktest
--
1. To generate `.ktest` file when symbolic variables are created using the API `klee_make_symbolic`  
  `gen-ktest --sym-vars seed.txt seed.ktest`

2. To generate `.ktest` file when the input file is symbolic  
  `gen-ktest --sym-file A seed.txt seed.ktest`  
   The tool will generate a input file with name `A` from the content of `hello.txt`.  
   
load-ktest
--   
1. From the `.ktest` file (often output of KLEE), grab the content of the file  
  `load-ktest seed.ktest seed.txt`  
