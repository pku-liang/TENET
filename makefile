CC := g++
SRCDIR := src
BUILDDIR := build
BINDIR := bin
TESTDIR := test

STD := -std=c++17

SOURCES := $(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))
LIB_DIR = external/lib
INCLUDE_DIR = external/include
LIB := -lbarvinok -lisl -lntl -lpolylibgmp -lgmp
INC := -I include -I . -I ${INCLUDE_DIR}
LOAD := -L ${LIB_DIR}

MAIN = main.cpp
TARGET = alexnet
LOG = $(TARGET)_log

CLI = cli.cpp
CLI_TARGET = tenet
CLI_LOG = $(CLI_TARGET)_log

HASCO = HASCO_interface.cpp
HASCO_TARGET = HASCO_interface
HASCO_LOG = ${HASCO_TARGET}_log

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(BUILDDIR)
	@echo "compile $< into object file..."
	@echo "$(CC) $(STD) -c $(INC) -o $@ $<" >> $(LOG)
	@$(CC) $(STD) -c $(INC) -o $@ $< >> $(LOG) 2>&1

clean:
	@echo "clean up bin, build directory"
	@rm -rf $(BUILDDIR)
	@rm -rf $(BINDIR)
	@rm -f "config.h"
	@echo "clean up $(TARGET) log file"
	@rm -rf $(LOG)

all: $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "compile entry file and link with tenet and external library..."
	@echo "#define EXPERIMENT_PREFIX \"${TARGET}\"" > config.h
	@echo "$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(MAIN) -o $(BINDIR)/$(TARGET) $(LIB) " > $(LOG)
	@$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(MAIN) -o $(BINDIR)/$(TARGET) $(LIB) >> $(LOG) 2>&1

cli: $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "compile command line interface and link with tenet and external library..."
	@echo "$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(CLI) -o $(BINDIR)/$(CLI_TARGET) $(LIB) " > $(CLI_LOG)
	@$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(CLI) -o $(BINDIR)/$(CLI_TARGET) $(LIB) >> $(CLI_LOG) 2>&1

HASCO: $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo "compile HASCO Interface and link with tenet and external library..."
	@echo "$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(HASCO) -o $(BINDIR)/$(HASCO_TARGET) $(LIB) " > ${HASCO_LOG}
	@$(CC) $(STD) $(INC) $(LOAD) $(OBJECTS) $(TESTDIR)/$(HASCO) -o $(BINDIR)/$(HASCO_TARGET) $(LIB) >> ${HASCO_LOG} 2>&1

.PHONY: clean all
