CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c11
SRC     = src/main.c src/md5.c src/sha256.c src/crack.c
TARGET  = hashcrack

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean
