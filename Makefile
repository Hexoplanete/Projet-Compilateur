include makeconfig/config.mk

MAIN=ifcc
SRC_DIR=compiler
BUILD_DIR=build
GEN_DIR=generated
INCLUDE_DIR=$(ANTLRINC)
LIBS=$(ANTLRLIB)

FILES=$(shell find $(SRC_DIR) -name '*.cpp')

GENERATED = $(SRC_DIR)/$(GEN_DIR)/ifccLexer.cpp $(SRC_DIR)/$(GEN_DIR)/ifccVisitor.cpp $(SRC_DIR)/$(GEN_DIR)/ifccBaseVisitor.cpp $(SRC_DIR)/$(GEN_DIR)/ifccParser.cpp
FILES += $(GENERATED)

CC=g++
CCFLAGS+=-g -std=c++17 -Wno-attributes # -Wno-defaulted-function-deleted -Wno-unknown-warning-option
LDFLAGS=-g

INCLUDE_ARGS = $(INCLUDE_DIR:%=-I%)
OBJECTS=$(FILES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
default: all
all: $(MAIN)

$(MAIN): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $(shell find $(BUILD_DIR) -name '*.o') $(LIBS) -o $(MAIN)
	

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/$(GEN_DIR)/ifccParser.cpp
	@mkdir -p $(dir $(<:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o))
	$(CC) -c $(INCLUDE_ARGS) $(CCFLAGS) -MMD -o $@ $< 

-include $(BUILD_DIR)/*.d
$(BUILD_DIR)/%.d:

$(GENERATED): $(SRC_DIR)/ifcc.g4
	@mkdir -p $(SRC_DIR)/$(GEN_DIR)
	cd $(SRC_DIR) && $(ANTLR) -visitor -no-listener -Dlanguage=Cpp -o $(GEN_DIR) ifcc.g4

# prevent automatic cleanup of "intermediate" files like ifccLexer.cpp etc
.PRECIOUS: $(GENERATED)  

# Usage: `make gui FILE=path/to/your/file.c`
FILE ?= ../tests/testfiles/1_return42.c

test:
	cd tests && python3 ifcc-test.py testfiles

gui:
	@mkdir -p $(SRC_DIR)/$(GEN_DIR) $(BUILD_DIR)
	$(ANTLR) -Dlanguage=Java -o $(SRC_DIR)/$(GEN_DIR) $(SRC_DIR)/ifcc.g4
	javac -cp $(ANTLRJAR) -d $(BUILD_DIR) $(SRC_DIR)/$(GEN_DIR)/*.java
	java -cp $(ANTLRJAR):$(BUILD_DIR) org.antlr.v4.gui.TestRig ifcc axiom -gui $(FILE)


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm -r $(SRC_DIR)/$(GEN_DIR)
	rm $(MAIN)
