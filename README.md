# klee-utils
Some utils for the KLEE symbolic execution engine

2. To generate `.ktest` file when symbolic variables are created using the API `klee_make_symbolic`  
  `gen-ktest --sym-vars hello.txt seed.ktest`

3. To generate `.ktest` file when the input file is symbolic  
  `gen-ktest --sym-file A hello.txt seed.ktest`  
   The tool will generate a input file with name `A` from the content of `hello.txt`.
