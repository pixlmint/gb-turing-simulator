# Variables
GBDK_HOME = ./gbdk/
LCC = $(GBDK_HOME)bin/lcc
PROJECTNAME = GB-Turing-Simulator
BINS = $(PROJECTNAME).gb
CSOURCES = $(filter-out tests/%, $(wildcard *.c))
ASMSOURCES = $(wildcard *.s)
TEST_SOURCES = $(wildcard tests/*.c)
TEST_EXEC = tests/test

# Default rule
all: $(BINS)

# Rule for creating the executable
$(BINS): $(CSOURCES) $(ASMSOURCES)
	$(LCC) -o $@ $(CSOURCES) $(ASMSOURCES)


# Rule for running tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(TEST_SOURCES)
	gcc -o $@ $^ -lcunit

# Rule for cleaning the project
clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm