STRUCTURES_PATH = src/structures/

SOURCE_INDEX  = $(STRUCTURES_PATH)index.cpp
SOURCE_COLUMN = $(STRUCTURES_PATH)column.cpp
SOURCE_TABLE  = $(STRUCTURES_PATH)table.cpp
SOURCE_DATABASE = $(STRUCTURES_PATH)database.cpp
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -ll -lfl
OBJECT_INDEX  = ${SOURCE_INDEX:.cpp=.o}
OBJECT_COLUMN = ${SOURCE_COLUMN:.cpp=.o}
OBJECT_TABLE  = ${SOURCE_TABLE:.cpp=.o}
OBJECT_DATABASE  = ${SOURCE_DATABASE:.cpp=.o}

MAIN=main

ech=echo

OBJECT_INDEX:
	$(CXX) -c $(SOURCE_INDEX)

OBJECT_COLUMN:
	$(CXX) -c $(SOURCE_COLUMN)

OBJECT_TABLE:
	$(CXX) -c $(SOURCE_TABLE)

OBJECT_DATABASE:
	$(CXX) -c $(SOURCE_DATABASE)

all: 
	$(ech)
#$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJECT_INDEX)



#$(PROJECT1): $(OBJECTS1)
#	$(CC) $(OBJECTS1) -o $(PROJECT1) -lboost_system -lcrypto -lssl -lcpprest -lpthread -std=c17


clean:
	-rm -f $(OBJECT_INDEX) *.o
	
