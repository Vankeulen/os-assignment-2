echo COMPILING WITH -WALL && gcc -o main main.c -Wall && gcc -o child child.c -Wall && echo CLANG TIDYING && clang-tidy main.c child.c -checks=*  --  main.c child.c && echo RUNNING WITH VALGRIND && valgrind ./main