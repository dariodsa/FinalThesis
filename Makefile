STRUCTURES_PATH = src/structures/
DB_PATH = src/db/
ALG_PATH = src/algorithm/
OPERATION_PATH = src/structures/operations/

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

SOURCE_NETWORK = $(ALG_PATH)network.cpp

SOURCE_INDEXCON = $(OPERATION_PATH)indexcon.cpp
SOURCE_INDEXSCAN = $(OPERATION_PATH)indexscan.cpp
SOURCE_OPERATION = $(OPERATION_PATH)operation.cpp
SOURCE_RETRDATA = $(OPERATION_PATH)retrdata.cpp
SORUCE_SEQSCAN = $(OPERATION_PATH)seqscan.cpp

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

OBJECT_NETWORK = ${SOURCE_NETWORK:.cpp=.o}

OBJECT_INDEXCON = ${SOURCE_INDEXCON:.cpp=.o}
OBJECT_INDEXSCAN = ${SOURCE_INDEXSCAN:.cpp=.o}
OBJECT_OPERATION = ${SOURCE_OPERATION:.cpp=.o}
OBJECT_RETRDATA = ${SOURCE_RETRDATA:.cpp=.o}
OBJECT_SEQSCAN = ${SORUCE_SEQSCAN:.cpp=.o}

OBJECT_PROGRAM = ${SOURCE_PROGRAM:.cpp=.o}
OBJECT_CONFIG = ${SOURCE_CONFIG:.cpp=.o}
MAIN=main
 
main: lex_yacc_part network operations index column table result database program proxy  config
	$(CXX) $(CXXFLAGS)  -o $@  src/main.cpp  $(OBJECT_NETWORK) $(OBJECT_SEQSCAN) $(OBJECT_RETRDATA) $(OBJECT_INDEXCON) $(OBJECT_INDEXSCAN) $(OBJECT_OPERATION) $(OBJECT_PROXY) $(OBJECT_TOKEN) $(OBJECT_CONFIG)  $(OBJECT_PROGRAM) $(OBJECT_RESULT) $(OBJECT_DATABASE) $(OBJECT_TABLE) $(OBJECT_COLUMN) $(OBJECT_INDEX)  $(LEX_YACC_FLAGS)  $(POSTGE_FLAGS)

operations: indexcon indexscan retrdata seqscan
	$(CXX) $(CXXFLAGS) -c $(SOURCE_OPERATION) -o $(OBJECT_OPERATION)
indexcon:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_INDEXCON) -o $(OBJECT_INDEXCON)
indexscan:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_INDEXSCAN) -o $(OBJECT_INDEXSCAN)
retrdata:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_RETRDATA) -o $(OBJECT_RETRDATA)
seqscan:
	$(CXX) $(CXXFLAGS) -c $(SORUCE_SEQSCAN) -o $(OBJECT_SEQSCAN)


network:
	$(CXX) $(CXXFLAGS) -c $(SOURCE_NETWORK) -o $(OBJECT_NETWORK)
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
	
