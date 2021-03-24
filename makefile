CC := g++
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TESTDIR := test

STD := -std=c++17

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
LIB := -lbarvinok -lisl -lntl -lpolylibgmp -lgmp
INC := -I include -I external/include
LOAD := -L external/lib

MAIN = main.cpp
TARGET = a.out
LOG = $(TARGET)_log

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@echo "compile $< into object file..."
	@echo "$(CC) $(STD) -c $(INC) -o $@ $<" >> $(LOG)
	@$(CC) $(STD) -c $(INC) -o $@ $< >> $(LOG) 2>&1

clean:
	@echo "clean up bin, build directory"
	@rm -rf $(BUILDDIR)
	@rm -rf $(BINDIR)
	@echo "clean up $(TARGET) log file"
	@rm -rf $(LOG)

all: $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "compile entry file and link with tenet and external library..."
	@echo "$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(MAIN) -o $(BINDIR)/$(TARGET) $(LIB) " >> $(LOG)
	@$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(MAIN) -o $(BINDIR)/$(TARGET) $(LIB) >> $(LOG) 2>&1
.PHONY: clean all
