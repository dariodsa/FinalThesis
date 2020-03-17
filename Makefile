STRUCTURES_PATH = src/structures/
DB_PATH = src/db/

DATA_PATH = data/

LEX_FILE = sql.l
YACC_FILE = sql.y

SOURCE_INDEX    = $(STRUCTURES_PATH)index.cpp
SOURCE_COLUMN   = $(STRUCTURES_PATH)column.cpp
SOURCE_TABLE    = $(STRUCTURES_PATH)table.cpp
SOURCE_DATABASE = $(STRUCTURES_PATH)database.cpp
SOURCE_RESULT   = $(STRUCTURES_PATH)result.cpp


SOURCE_PROGRAM = $(DB_PATH)program.cpp
SOURCE_CONFIG  = $(DB_PATH)config.cpp
SOURCE_PROXY   = $(DB_PATH)proxy.cpp

SOURCE_MAIN = src/main.cpp

CXX = g++
CXXFLAGS =  -g -std=c++11 -lcpprest
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
OBJECT_PROXY     = ${SOURCE_PROXY:.cpp=.o}
OBJECT_RESULT = ${SOURCE_RESULT:.cpp=.o}

OBJECT_PROGRAM = ${SOURCE_PROGRAM:.cpp=.o}
OBJECT_CONFIG = ${SOURCE_CONFIG:.cpp=.o}
MAIN=main
 
main: lex_yacc_part index column table result database program proxy  config
	$(CXX) $(CXXFLAGS)  -o $@  src/main.cpp  $(OBJECT_PROXY) $(OBJECT_TOKEN) $(OBJECT_CONFIG)  $(OBJECT_PROGRAM) $(OBJECT_RESULT) $(OBJECT_DATABASE) $(OBJECT_TABLE) $(OBJECT_COLUMN) $(OBJECT_INDEX)  $(LEX_YACC_FLAGS)  $(POSTGE_FLAGS)

index: 
	$(CXX) $(CXXFLAGS) -c $(SOURCE_INDEX) -o $(OBJECT_INDEX)

column:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_COLUMN) -o $(OBJECT_COLUMN)

table:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_TABLE) -o $(OBJECT_TABLE)

database: 
	$(CXX) $(CXXFLAGS) -c $(SOURCE_DATABASE) -o $(OBJECT_DATABASE)

result:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_RESULT) -o $(OBJECT_RESULT)

program: 
	$(CXX) $(CXXFLAGS) -c $(SOURCE_PROGRAM) -o $(OBJECT_PROGRAM)

config: 
	$(CXX)  $(CXXFLAGS) -c $(SOURCE_CONFIG)  -o $(OBJECT_CONFIG)  $(LEX_YACC_FLAGS)

token: 
	$(CXX)  $(CXXFLAGS) -c $(SOURCE_TOKEN)  -o $(OBJECT_TOKEN)


proxy:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_PROXY) -o $(OBJECT_PROXY)

lex_yacc_part:
	cd data; $(YACC) $(YACCFLAGS) $(YACC_FILE)
	cd data; $(LEX) $(LEXFLAGS) $(LEX_FILE)
	cd data; sed -i '1s/^/#include "..\/src\/structures\/index.h"\n/' y.tab.h



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
	
