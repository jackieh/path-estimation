CXX = g++
TOP = .
SRC = ./src
FLAGS = -g -Wall -Werror -Wextra -Weffc++ -DDEBUG

# Library (default target)
LIB_DIR = $(SRC)/pathest
LIB_OUT = $(TOP)/libpathest.a
LIB_FLAGS = -I$(SRC) $(FLAGS)
LIB_SOURCES = \
	$(LIB_DIR)/path_data.cc \
	$(LIB_DIR)/estimation.cc
LIB_OBJECTS = $(LIB_SOURCES:.cc=.o)

# Test (test target)
TEST_DIR = $(SRC)/test
TEST_OUT = $(TOP)/estimate
TEST_LIBS = -L$(TOP) -lplplotd -lpathest -ljsoncpp -larmadillo
TEST_FLAGS = -I$(SRC) -isystem/usr/include/jsoncpp $(FLAGS)
TEST_SOURCES = \
	$(TEST_DIR)/analysis.cc \
	$(TEST_DIR)/results.cc \
	$(TEST_DIR)/parse.cc \
	$(TEST_DIR)/main.cc
TEST_OBJECTS = $(TEST_SOURCES:.cc=.o)

all: $(LIB_OUT)

test: $(LIB_OUT) $(TEST_OUT)

$(LIB_OUT): $(LIB_OBJECTS)
	rm -f $@
	ar cq $@ $(LIB_OBJECTS)

$(TEST_OUT): $(TEST_OBJECTS)
	$(CXX) -o $@ $(TEST_OBJECTS) $(TEST_LIBS)

$(LIB_DIR)/%.o: CXX_FLAGS := $(LIB_FLAGS)
$(TEST_DIR)/%.o: CXX_FLAGS := $(TEST_FLAGS)

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

clean:
	rm -f $(LIB_OUT)
	rm -f $(LIB_OBJECTS)
	rm -f $(TEST_OUT)
	rm -f $(TEST_OBJECTS)

docs:
	doxygen doxygen.conf

run:
	mkdir -p $(TOP)/test/out/tmp
	$(TOP)/estimate $(TOP)/test/config.json \
	  $(TOP)/test/data/given/reports.txt $(TOP)/test/out/tmp
