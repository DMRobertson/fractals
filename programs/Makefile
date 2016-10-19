HEADER_DIR = include
SOURCE_DIR = src
OBJECT_DIR = build
OUTPUT_DIR = bin

SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(OBJECT_DIR)/%.o, $(SOURCES))
HEADERS = $(wildcard $(HEADER_DIR)/*.h)

CC = gcc
CFLAGS = -std=c99 -g -Wall -Wextra -I $(HEADER_DIR)
CLIBS  = -lm

.PHONY: binary 
binary: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(CLIBS) -o $(OUTPUT_DIR)/koch

# -c: compile only
# -o: output to
# $@ target (%.o)
# $< 1st dependancy (%.c)
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean print-% run debug memtest
clean:
	rm -f $(OBJECT_DIR)/*.o $(OUTPUT_DIR)/*

run: binary
	./$(OUTPUT_DIR)/koch

debug: binary
	gdb ./$(OUTPUT_DIR)/koch

memtest: binary
	valgrind ./$(OUTPUT_DIR)/koch
#for debugging the Makefile
print-%  : ; @echo $* = $($*)