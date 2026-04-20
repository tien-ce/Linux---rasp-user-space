# --- Configuration ---
# Match the architecture and compiler from your kernel Makefile 
ARCH = arm64
CROSS_COMPILE = aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc

# Target binary name
TARGET = main_app

# Source files
SRC = main.c
OBJ = $(SRC:.c=.o)

# Compilation flags
CFLAGS = -Wall -Wextra -O2
LDFLAGS = 

# --- Build Rules ---

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

# Use your existing push_rpi logic to move the binary 
install: $(TARGET)
	push_user_rpi $(TARGET)
