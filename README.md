```bash
yacc -d sql.y && lex sql.l && gcc -o main lex.yy.c y.tab.c -ly -ll && ./main <i1
```     

```bash
g++ -I ../data -o main main.cpp db/program.cpp structures/database.cpp -ll -lfl
```
