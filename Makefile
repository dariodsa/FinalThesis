STRUCTURES_PATH = src/structures/
DB_PATH = src/db/
ALG_PATH = src/algorithm/
OPERATION_PATH = src/structures/operations/
STRATEGIES_PATH = src/structures/strategies/

DATA_PATH = data/

LEX_FILE = sql.l
YACC_FILE = sql.y

SRC_INDEX    = $(STRUCTURES_PATH)index.cpp
SRC_COLUMN   = $(STRUCTURES_PATH)column.cpp
SRC_TABLE    = $(STRUCTURES_PATH)table.cpp
SRC_DATABASE = $(STRUCTURES_PATH)database.cpp
SRC_RESULT   = $(STRUCTURES_PATH)result.cpp
SRC_CACHE   = $(STRUCTURES_PATH)cache.cpp
SRC_FOREIGN_KEY = $(STRUCTURES_PATH)foreign-key.cpp

SRC_PROGRAM = $(DB_PATH)program.cpp
SRC_CONFIG  = $(DB_PATH)config.cpp
SRC_PROXY   = $(DB_PATH)proxy.cpp

SRC_NETWORK = $(ALG_PATH)network.cpp
SRC_TOPOSORT = $(ALG_PATH)toposort.cpp

SRC_INDEXCON = $(OPERATION_PATH)indexcon.cpp
SRC_INDEXSCAN = $(OPERATION_PATH)indexscan.cpp
SRC_OPERATION = $(OPERATION_PATH)operation.cpp
SRC_SEQSCAN = $(OPERATION_PATH)seqscan.cpp

SRC_HASH_JOIN = $(OPERATION_PATH)hash-join.cpp
SRC_MERGE_JOIN = $(OPERATION_PATH)merge-join.cpp
SRC_NESTED_JOIN = $(OPERATION_PATH)nested-join.cpp

SRC_LB = $(STRATEGIES_PATH)load-balance.cpp
SRC_RR = $(STRATEGIES_PATH)round-robin.cpp
SRC_LW = $(STRATEGIES_PATH)least-work.cpp
SRC_SMART = $(STRATEGIES_PATH)smart.cpp

SRC_FILTER = $(OPERATION_PATH)filter.cpp
SRC_SORT = $(OPERATION_PATH)sort.cpp
SRC_GROUP = $(OPERATION_PATH)group.cpp
SRC_OR_UNION = $(OPERATION_PATH)or-union.cpp

SRC_MAIN = src/main.cpp

CXX = g++
CXXFLAGS =  -g -std=c++11 -lcpprest -lpthread
LEX_YACC_FLAGS = -ll -lfl -ly
POSTGE_FLAGS = -lpqxx -lpq

LEX = lex
LEXFLAGS = 
YACC = yacc
YACCFLAGS = -d

OBJ_INDEX  = ${SRC_INDEX:.cpp=.o}
OBJ_COLUMN = ${SRC_COLUMN:.cpp=.o}
OBJ_TABLE  = ${SRC_TABLE:.cpp=.o}
OBJ_DATABASE  = ${SRC_DATABASE:.cpp=.o}
OBJ_PROXY     = ${SRC_PROXY:.cpp=.o}
OBJ_RESULT = ${SRC_RESULT:.cpp=.o}
OBJ_CACHE  = ${SRC_CACHE:.cpp=.o}
OBJ_FOREIGN_KEY = ${SRC_FOREIGN_KEY:.cpp=.o}

OBJ_NETWORK = ${SRC_NETWORK:.cpp=.o}
OBJ_TOPOSORT = ${SRC_TOPOSORT:.cpp=.o}
	

OBJ_INDEXCON = ${SRC_INDEXCON:.cpp=.o}
OBJ_INDEXSCAN = ${SRC_INDEXSCAN:.cpp=.o}
OBJ_OPERATION = ${SRC_OPERATION:.cpp=.o}
OBJ_SEQSCAN = ${SRC_SEQSCAN:.cpp=.o}

OBJ_LB = ${SRC_LB:.cpp=.o}
OBJ_RR = ${SRC_RR:.cpp=.o}
OBJ_LW = ${SRC_LW:.cpp=.o}
OBJ_SMART = ${SRC_SMART:.cpp=.o}

OBJ_HASH_JOIN = ${SRC_HASH_JOIN:.cpp=.o}
OBJ_MERGE_JOIN = ${SRC_MERGE_JOIN:.cpp=.o}
OBJ_NESTED_JOIN = ${SRC_NESTED_JOIN:.cpp=.o}
OBJ_FILTER = ${SRC_FILTER:.cpp=.o}
OBJ_SORT = ${SRC_SORT:.cpp=.o}
OBJ_GROUP = ${SRC_GROUP:.cpp=.o}
OBJ_OR_UNION = ${SRC_OR_UNION:.cpp=.o}

OBJ_PROGRAM = ${SRC_PROGRAM:.cpp=.o}
OBJ_CONFIG = ${SRC_CONFIG:.cpp=.o}
MAIN=main
 
main: lex_yacc_part cache balance network toposort operations index column table result database foreign-key program  config
	$(CXX) $(CXXFLAGS)  -o $@  src/main.cpp $(OBJ_LB) $(OBJ_RR)  $(OBJ_LW) $(OBJ_SMART) $(OBJ_TOPOSORT) $(OBJ_NESTED_JOIN) $(OBJ_FILTER) $(OBJ_SORT) $(OBJ_GROUP) $(OBJ_OR_UNION) $(OBJ_MERGE_JOIN) $(OBJ_HASH_JOIN) $(OBJ_FOREIGN_KEY) $(OBJ_CACHE) $(OBJ_NETWORK) $(OBJ_SEQSCAN)  $(OBJ_INDEXCON) $(OBJ_INDEXSCAN) $(OBJ_OPERATION) $(OBJ_PROXY) $(OBJ_TOKEN) $(OBJ_CONFIG)  $(OBJ_PROGRAM) $(OBJ_RESULT) $(OBJ_DATABASE) $(OBJ_TABLE) $(OBJ_COLUMN) $(OBJ_INDEX)  $(LEX_YACC_FLAGS)  $(POSTGE_FLAGS)

operations: indexcon indexscan seqscan hash-join merge-join nested-join filter sort group or-union
	$(CXX) $(CXXFLAGS) -c $(SRC_OPERATION) -o $(OBJ_OPERATION)

balance: rr lw smart
	$(CXX) $(CXXFLAGS) -c $(SRC_LB) -o $(OBJ_LB)

rr:
	$(CXX) $(CXXFLAGS) -c $(SRC_RR) -o $(OBJ_RR)

lw:
	$(CXX) $(CXXFLAGS) -c $(SRC_LW) -o $(OBJ_LW)

smart:
	$(CXX) $(CXXFLAGS) -c $(SRC_SMART) -o $(OBJ_SMART)

cache:
	$(CXX) $(CXXFLAGS) -c $(SRC_CACHE) -o $(OBJ_CACHE)

foreign-key: 
	$(CXX) $(CXXFLAGS) -c $(SRC_FOREIGN_KEY) -o $(OBJ_FOREIGN_KEY)

indexcon:
	$(CXX) $(CXXFLAGS) -c $(SRC_INDEXCON) -o $(OBJ_INDEXCON)

indexscan:
	$(CXX) $(CXXFLAGS) -c $(SRC_INDEXSCAN) -o $(OBJ_INDEXSCAN)

seqscan: $(SRC_SEQSCAN)
	$(CXX) $(CXXFLAGS) -c $(SRC_SEQSCAN) -o $(OBJ_SEQSCAN)

hash-join: $(SRC_HASH_JOIN)
	$(CXX) $(CXXFLAGS) -c $(SRC_HASH_JOIN) -o $(OBJ_HASH_JOIN)

merge-join: $(SRC_MERGE_JOIN)
	$(CXX) $(CXXFLAGS) -c $(SRC_MERGE_JOIN) -o $(OBJ_MERGE_JOIN)

nested-join: $(SRC_NESTED_JOIN) 
	$(CXX) $(CXXFLAGS) -c $(SRC_NESTED_JOIN) -o $(OBJ_NESTED_JOIN)

filter: $(SRC_FILTER)
	$(CXX) $(CXXFLAGS) -c $(SRC_FILTER) -o $(OBJ_FILTER)

sort: $(SRC_SORT)
	$(CXX) $(CXXFLAGS) -c $(SRC_SORT) -o $(OBJ_SORT)

group: $(SRC_GROUP)
	$(CXX) $(CXXFLAGS) -c $(SRC_GROUP) -o $(OBJ_GROUP)

or-union: $(SRC_OR_UNION)
	$(CXX) $(CXXFLAGS) -c $(SRC_OR_UNION) -o $(OBJ_OR_UNION)


network:
	$(CXX) $(CXXFLAGS) -c $(SRC_NETWORK) -o $(OBJ_NETWORK)

toposort:
	$(CXX) $(CXXFLAGS) -c $(SRC_TOPOSORT) -o $(OBJ_TOPOSORT)

index: 
	$(CXX) $(CXXFLAGS) -c $(SRC_INDEX) -o $(OBJ_INDEX)

column:
	$(CXX) $(CXXFLAGS) -c $(SRC_COLUMN) -o $(OBJ_COLUMN)

table:
	$(CXX) $(CXXFLAGS) -c $(SRC_TABLE) -o $(OBJ_TABLE)

database: 
	$(CXX) $(CXXFLAGS) -c $(SRC_DATABASE) -o $(OBJ_DATABASE)

result:
	$(CXX) $(CXXFLAGS) -c $(SRC_RESULT) -o $(OBJ_RESULT)

program: 
	$(CXX) $(CXXFLAGS) -c $(SRC_PROGRAM) -o $(OBJ_PROGRAM)

config: 
	$(CXX)  $(CXXFLAGS) -c $(SRC_CONFIG)  -o $(OBJ_CONFIG)  $(LEX_YACC_FLAGS)

token: 
	$(CXX)  $(CXXFLAGS) -c $(SRC_TOKEN)  -o $(OBJ_TOKEN)


lex_yacc_part:
	cd data; $(YACC) $(YACCFLAGS) $(YACC_FILE)
	cd data; $(LEX) $(LEXFLAGS) $(LEX_FILE)
	cd data; sed -i '1s/^/#include "..\/src\/structures\/index.h"\n/' y.tab.h



clean:
	-rm -f $(OBJ_INDEX)
	-rm -f $(OBJ_COLUMN)
	-rm -f $(OBJ_TABLE)
	-rm -f $(OBJ_DATABASE)
	-rm -f $(OBJ_PROGRAM)
	-rm -f data/lex.yy.c
	-rm -f data/y.tab.*
	-rm -f src/db/config.o
	-rm -f $(MAIN)
	-rm -f $(OBJ_NETWORK) 
	-rm -f $(OBJ_INDEXCON)
	-rm -f $(OBJ_INDEXSCAN)
	-rm -f $(OBJ_OPERATION)
	-rm -f $(OBJ_SEQSCAN)
	
