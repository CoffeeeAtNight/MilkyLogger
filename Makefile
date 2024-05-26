# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS := -Wall -Wextra -g

# define output directory
OUTPUT := output

# define source directory
SRC := src

# define include directory
INCLUDE := include

# define main executable name
MAIN := main.exe

# define sources and objects
SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c,$(SRC)/%.o,$(SOURCES))

# ensure output directory exists
$(OUTPUT):
	@if not exist $(OUTPUT) mkdir $(OUTPUT)

# build the main executable
$(OUTPUT)/$(MAIN): $(OBJECTS) | $(OUTPUT)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $(OBJECTS)

# build object files
$(SRC)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

# clean up generated files
.PHONY: clean
clean:
	@if exist $(SRC)\*.o del $(SRC)\*.o
	@if exist $(OUTPUT)\$(MAIN) del $(OUTPUT)\$(MAIN)
	@echo Cleanup complete!

# default target
.PHONY: all
all: $(OUTPUT)/$(MAIN)
	@echo Build complete!
