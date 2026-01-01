# Target must be cmpe351
TARGET   := cmpe351
# Is file .c or .cpp
SRC      := $(shell ls cmpe351.cpp 2>/dev/null || ls cmpe351.c 2>/dev/null)
# --- OS Detection ---
OS       := $(shell uname -s)
IS_POSIX := $(shell uname -s | grep -iE 'Linux|Darwin|Unix|BSD' > /dev/null && echo yes || echo no)

# --- Compiler Selection ---
# If the file ends in .cpp, use g++, otherwise use gcc
ifneq ($(filter %.cpp,$(SRC)),)
    CC := g++
    LANG_FLAGS := -std=c++17
    MSG := "C++ source detected. Using g++..."
else
    CC := gcc
    LANG_FLAGS := -std=c11
    MSG := "C source detected. Using gcc..."
endif

# --- Recipes ---
all: check_os $(TARGET)

check_os:
ifeq ($(IS_POSIX),no)
	@echo "ERROR: POSIX environment not detected (Found: $(OS))"
	@exit 1
endif

$(TARGET): $(SRC)
	@echo $(MSG)
	$(CC) $(SRC) $(LANG_FLAGS) -Wall -o $(TARGET)

clean:
	rm -f $(TARGET)

test: $(TARGET)
	bash test.sh

.PHONY: all check_os clean test
