CC = gcc
CFLAGS = -Wall -g -I/usr/lib/ssl/include
LDFLAGS = -L/usr/lib/ssl/lib -lssl -lcrypto

SRC_PROGRAM1 = ecb.c bmp_utils.c
SRC_PROGRAM2 = ecb_decrypt.c bmp_utils.c
SRC_PROGRAM3 = cbc.c bmp_utils.c
SRC_PROGRAM4 = cbc_decrypt.c bmp_utils.c

OBJ_PROGRAM1 = $(SRC_PROGRAM1:.c=.o)
OBJ_PROGRAM2 = $(SRC_PROGRAM2:.c=.o)
OBJ_PROGRAM3 = $(SRC_PROGRAM3:.c=.o)
OBJ_PROGRAM4 = $(SRC_PROGRAM4:.c=.o)

TARGET1 = ecb_processor
TARGET2 = ecb_decrypt_processor
TARGET3 = cbc_processor
TARGET4 = cbc_decrypt_processor

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

ecb: $(TARGET1) $(TARGET2)

cbc: $(TARGET3) $(TARGET4)

$(TARGET1): $(OBJ_PROGRAM1)
	$(CC) $(OBJ_PROGRAM1) -o $@ $(LDFLAGS)

$(TARGET2): $(OBJ_PROGRAM2)
	$(CC) $(OBJ_PROGRAM2) -o $@ $(LDFLAGS)

$(TARGET3): $(OBJ_PROGRAM3)
	$(CC) $(OBJ_PROGRAM3) -o $@ $(LDFLAGS)

$(TARGET4): $(OBJ_PROGRAM4)
	$(CC) $(OBJ_PROGRAM4) -o $@ $(LDFLAGS)	

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *_processor
