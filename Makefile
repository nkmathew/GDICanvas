## Some variables for convenience purposes e.g when changing folder names.
CXX_FLAGS   = -lstdc++ -Wall -pedantic -Wextra -std=c++11 -mwindows -lwinmm
CC          = gcc
SRC_DIR     = src
BUILD_DIR   = build
DEMO_DIR    = examples
TEST        = test
DEMO_RC     = logo
LIB_DIR     = $(BUILD_DIR)/lib
INCLUDE_DIR = $(BUILD_DIR)/include
LIBRARY     = $(LIB_DIR)/libGDICanvas.a
INCLUDES    = $(patsubst $(SRC_DIR)/%.h, $(INCLUDE_DIR)/%.h, $(wildcard $(SRC_DIR)/*.h))
DEMOS       = $(patsubst $(DEMO_DIR)/%.cxx, $(DEMO_DIR)/%.exe, $(wildcard $(DEMO_DIR)/*.cxx)) $(LIB_DIR)/$(DEMO_RC).o
OBJECTS     = $(LIB_DIR)/$(DEMO_RC).o
OBJECTS    += $(patsubst $(SRC_DIR)/%.cxx, $(LIB_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cxx))

ifdef DEBUG
CXX_FLAGS += -g -pg -DDEBUG
else
CXX_FLAGS += -s -O -DNDEBUG
endif

test:$(TEST).exe
	$(TEST).exe

$(TEST).exe:$(LIBRARY) $(INCLUDES) $(TEST).cxx
	$(CC) -I$(INCLUDE_DIR) $(TEST).cxx -lGDICanvas $(CXX_FLAGS) -L$(LIB_DIR) -o $(TEST).exe

all:lib $(DEMOS) $(TEST).exe

lib:$(OBJECTS) $(INCLUDES) $(LIBRARY)

## Library
$(LIBRARY): $(OBJECTS)
	ar rc $@ $^

## Logo resource
$(LIB_DIR)/$(DEMO_RC).o:$(SRC_DIR)/$(DEMO_RC).rc $(SRC_DIR)/$(DEMO_RC).h
	windres $(SRC_DIR)/$(DEMO_RC).rc $(LIB_DIR)/$(DEMO_RC).o

## Demos
$(DEMO_DIR)/%.exe:$(DEMO_DIR)/%.cxx $(LIB_DIR)/$(DEMO_RC).o $(LIBRARY)
	$(CC) -I$(INCLUDE_DIR) $< -lGDICanvas $(CXX_FLAGS) $(LIB_DIR)/$(DEMO_RC).o -L$(LIB_DIR) -o $@

## Vec2D.o
$(LIB_DIR)/Vec2D.o:$(SRC_DIR)/Vec2D.cxx $(SRC_DIR)/Vec2D.h
	$(CC) -c $< $(CXX_FLAGS) -o $@

## Shapes.o
$(LIB_DIR)/Shapes.o:$(SRC_DIR)/Shapes.cxx $(SRC_DIR)/Shapes.h $(LIB_DIR)/Vec2D.o
	$(CC) -c $< $(CXX_FLAGS) -o $@

## Canvas.o
$(LIB_DIR)/Canvas.o:$(SRC_DIR)/Canvas.cxx $(SRC_DIR)/Canvas.h $(LIB_DIR)/$(DEMO_RC).o \
						$(LIB_DIR)/Vec2D.o $(LIB_DIR)/Shapes.o $(LIB_DIR)/Colors.o
	$(CC) -c $< $(CXX_FLAGS) -o $@

## Colors.o
$(LIB_DIR)/Colors.o:$(SRC_DIR)/Colors.cxx $(SRC_DIR)/Colors.h
	$(CC) -c $< $(CXX_FLAGS) -o $@

## Header files
$(INCLUDE_DIR)/%.h:$(SRC_DIR)/%.h
	cp $< $(INCLUDE_DIR)

clean:
	rm -f $(LIB_DIR)/*.o $(LIBRARY) $(INCLUDE_DIR)/*.h
	rm -f *.exe $(DEMOS)

