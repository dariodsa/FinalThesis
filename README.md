```bash
yacc -d sql.y && lex sql.l && gcc -o main lex.yy.c y.tab.c -ly -ll && ./main <i1
```
