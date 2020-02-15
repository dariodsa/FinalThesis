STRUCTURES_PATH = src/structures/
DB_PATH = src/db/

DATA_PATH = 

LEX_FILE = $(DATA_PATH)sql.l
YACC_FILE = $(DATA_PATH)sql.y

SOURCE_INDEX  = $(STRUCTURES_PATH)index.cpp
SOURCE_COLUMN = $(STRUCTURES_PATH)column.cpp
SOURCE_TABLE  = $(STRUCTURES_PATH)table.cpp
SOURCE_DATABASE = $(STRUCTURES_PATH)database.cpp

SOURCE_PROGRAM = $(DB_PATH)program.cpp
SOURCE_CONFIG = $(DB_PATH)config.cpp

SOURCE_MAIN = src/main.cpp

CXX = g++
CXXFLAGS =  -g -std=c++11 
LEX_YACC_FLAGS = -ll -lfl -ly
POSTGE_FLAGS = -lpqxx -lpq

LEX = lex
LEXFLAGS = 
YACC = yacc
YACCFLAGS = -d 

OBJECT_INDEX  = ${SOURCE_INDEX:.cpp=.o}
OBJECT_COLUMN = ${SOURCE_COLUMN:.cpp=.o}
OBJECT_TABLE  = ${SOURCE_TABLE:.cpp=.o}
OBJECT_DATABASE  = ${SOURCE_DATABASE:.cpp=.o}

OBJECT_PROGRAM = ${SOURCE_PROGRAM:.cpp=.o}
OBJECT_CONFIG = ${SOURCE_CONFIG:.cpp=.o}

MAIN=main

OBJECT_INDEX: $(SOURCE_INDEX)
	$(CXX) -c $(SOURCE_INDEX) -o $(OBJECT_INDEX)

OBJECT_COLUMN:
	$(CXX) -c $(SOURCE_COLUMN) -o $(OBJECT_COLUMN)

OBJECT_TABLE:
	$(CXX) -c $(SOURCE_TABLE) -o $(OBJECT_TABLE)

OBJECT_DATABASE: 
	$(CXX) -c $(SOURCE_DATABASE) -o $(OBJECT_DATABASE)

OBJECT_PROGRAM: 
	$(CXX) -c $(SOURCE_PROGRAM) -o $(OBJECT_PROGRAM)

OBJECT_CONFIG:
	$(CXX)  -o $(OBJECT_CONFIG) -c $(SOURCE_CONFIG) $(LEX_YACC_FLAGS)

lex_yacc_part:
	cd data; $(YACC) $(YACCFLAGS) $(YACC_FILE)
	cd data; $(LEX) $(LEXFLAGS) $(LEX_FILE)
	cd data; sed -i '1s/^/#include "..\/src\/structures\/index.h"\n/' y.tab.h

all: lex_yacc_part OBJECT_INDEX OBJECT_COLUMN OBJECT_TABLE OBJECT_DATABASE OBJECT_PROGRAM OBJECT_CONFIG
	$(CXX) $(CXXFLAGS)  -I data -o $(MAIN) $(SOURCE_CONFIG) src/main.cpp   $(OBJECT_INDEX) $(OBJECT_PROGRAM) $(OBJECT_DATABASE) $(OBJECT_TABLE) $(OBJECT_COLUMN) $(LEX_YACC_FLAGS)  $(POSTGE_FLAGS)


clean:
	-rm -f $(OBJECT_INDEX)
	-rm -f $(OBJECT_COLUMN)
	-rm -f $(OBJECT_TABLE)
	-rm -f $(OBJECT_DATABASE)
	-rm -f $(OBJECT_PROGRAM)
	-rm -f data/lex.yy.c
	-rm -f data/y.tab.*
	-rm -f src/db/config.o
	-rm -f $(MAIN)
	
