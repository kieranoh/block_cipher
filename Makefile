CC = gcc
CFLAGS = -Wall -g -I/usr/lib/ssl/include
LDFLAGS = -L/usr/lib/ssl/lib -lssl -lcrypto

SRC = ecb.c bmp_utils.c
OBJ = $(SRC:.c=.o)
TARGET = ecb_bmp_processor

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
