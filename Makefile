CC = gcc

CFLAGS = -Wall -g -I/usr/lib/ssl/include
LDFLAGS = -L//usr/lib/ssl/lib -lssl -lcrypto

SRC = ecb.c
TARGET = ecb_bmp_processor

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
