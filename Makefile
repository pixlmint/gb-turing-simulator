# Variables
GBDK_HOME = ./gbdk/
LCC = $(GBDK_HOME)bin/lcc
PROJECTNAME = GB-Turing-Simulator
BINS = build/$(PROJECTNAME).gb
CSOURCES = $(filter-out tests/%, $(wildcard *.c))
ASMSOURCES = $(wildcard *.s)
TEST_SOURCES = $(wildcard tests/*.c)
TEST_EXEC = tests/test

# Default rule
all: $(BINS)

# Rule for creating the executable
$(BINS): $(CSOURCES) $(ASMSOURCES)
	mkdir -p build
	$(LCC) -Wl-m -msm83:gb -o $@ $(CSOURCES) $(ASMSOURCES)


# Rule for running tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(TEST_SOURCES)
	gcc -o $@ $^ -lcunit -I/usr/include/CUnit

play: all
	vba --throttle=100 --filter-super-eagle build/$(PROJECTNAME).gb

# Rule for cleaning the project
clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm, *.gb*
	rm -r build