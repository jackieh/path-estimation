CXX = g++
TOP = .
SRC = ./src
INC = -I$(SRC) -I/usr/include/jsoncpp
FLAGS = $(INC) -g -Wall -Werror -DDEBUG

# Library (default target)
CXX_LIB_DIR = pathest
CXX_LIB_OUT = $(TOP)/libpathest.a
CXX_LIB_SOURCES = \
	$(SRC)/$(CXX_LIB_DIR)/path_data.cc \
	$(SRC)/$(CXX_LIB_DIR)/estimation.cc
CXX_LIB_OBJECTS = $(CXX_LIB_SOURCES:.cc=.o)

# Test (test target)
CXX_TEST_DIR = test
CXX_TEST_OUT = $(TOP)/estimate
CXX_TEST_LIBS = -L$(TOP) -lplplotd -lpathest -ljsoncpp -larmadillo
CXX_TEST_SOURCES = \
	$(SRC)/$(CXX_TEST_DIR)/analysis.cc \
	$(SRC)/$(CXX_TEST_DIR)/results.cc \
	$(SRC)/$(CXX_TEST_DIR)/parse.cc \
	$(SRC)/$(CXX_TEST_DIR)/main.cc
CXX_TEST_OBJECTS = $(CXX_TEST_SOURCES:.cc=.o)

all: $(CXX_LIB_OUT)

test: $(CXX_LIB_OUT) $(CXX_TEST_OUT)

$(CXX_LIB_OUT): $(CXX_LIB_OBJECTS)
	rm -f $@
	ar cq $@ $(CXX_LIB_OBJECTS)

$(CXX_TEST_OUT): $(CXX_TEST_OBJECTS)
	$(CXX) -o $@ $(CXX_TEST_OBJECTS) $(CXX_TEST_LIBS)

%.o: %.cc
	$(CXX) $(FLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(CXX_LIB_OUT)
	rm -f $(CXX_LIB_OBJECTS)
	rm -f $(CXX_TEST_OUT)
	rm -f $(CXX_TEST_OBJECTS)

docs:
	doxygen doxygen.conf

run:
	mkdir -p $(TOP)/test/out/tmp
	$(TOP)/estimate $(TOP)/test/config.json \
	  $(TOP)/test/data/given/reports.txt $(TOP)/test/out/tmp
