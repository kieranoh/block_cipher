CC = gcc
CFLAGS = -Wall -g -I/usr/lib/ssl/include
LDFLAGS = -L/usr/lib/ssl/lib -lssl -lcrypto

SRC_PROGRAM1 = ecb.c bmp_utils.c
SRC_PROGRAM2 = ecb_decrypt.c bmp_utils.c
SRC_PROGRAM3 = cbc.c bmp_utils.c
SRC_PROGRAM4 = cbc_decrypt.c bmp_utils.c
SRC_PROGRAM5 = ctr.c bmp_utils.c
SRC_PROGRAM6 = ctr_decrypt.c bmp_utils.c
SRC_PROGRAM7 = pcbc.c bmp_utils.c
SRC_PROGRAM8 = pcbc_decrypt.c bmp_utils.c
SRC_PROGRAM9 = cfb.c bmp_utils.c
SRC_PROGRAM10 = cfb_decrypt.c bmp_utils.c
SRC_PROGRAM11 = ofb.c bmp_utils.c
SRC_PROGRAM12 = ofb_decrypt.c bmp_utils.c

OBJ_PROGRAM1 = $(SRC_PROGRAM1:.c=.o)
OBJ_PROGRAM2 = $(SRC_PROGRAM2:.c=.o)
OBJ_PROGRAM3 = $(SRC_PROGRAM3:.c=.o)
OBJ_PROGRAM4 = $(SRC_PROGRAM4:.c=.o)
OBJ_PROGRAM5 = $(SRC_PROGRAM5:.c=.o)
OBJ_PROGRAM6 = $(SRC_PROGRAM6:.c=.o)
OBJ_PROGRAM7 = $(SRC_PROGRAM7:.c=.o)
OBJ_PROGRAM8 = $(SRC_PROGRAM8:.c=.o)
OBJ_PROGRAM9 = $(SRC_PROGRAM9:.c=.o)
OBJ_PROGRAM10 = $(SRC_PROGRAM10:.c=.o)
OBJ_PROGRAM11 = $(SRC_PROGRAM11:.c=.o)
OBJ_PROGRAM12 = $(SRC_PROGRAM12:.c=.o)

TARGET1 = ecb_processor
TARGET2 = ecb_decrypt_processor
TARGET3 = cbc_processor
TARGET4 = cbc_decrypt_processor
TARGET5 = ctr_processor
TARGET6 = ctr_decrypt_processor
TARGET7 = pcbc_processor
TARGET8 = pcbc_decrypt_processor
TARGET9 = cfb_processor
TARGET10 = cfb_decrypt_processor
TARGET11 = ofb_processor
TARGET12 = ofb_decrypt_processor


all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7) $(TARGET8) $(TARGET9) $(TARGET10) $(TARGET11) $(TARGET12)

ecb: $(TARGET1) $(TARGET2)

cbc: $(TARGET3) $(TARGET4)

ctr: $(TARGET5) $(TARGET6)

pcbc: $(TARGET7) $(TARGET8)

cfb: $(TARGET9) $(TARGET10)

ofb: $(TARGET11) $(TARGET12)

$(TARGET1): $(OBJ_PROGRAM1)
	$(CC) $(OBJ_PROGRAM1) -o $@ $(LDFLAGS)

$(TARGET2): $(OBJ_PROGRAM2)
	$(CC) $(OBJ_PROGRAM2) -o $@ $(LDFLAGS)

$(TARGET3): $(OBJ_PROGRAM3)
	$(CC) $(OBJ_PROGRAM3) -o $@ $(LDFLAGS)

$(TARGET4): $(OBJ_PROGRAM4)
	$(CC) $(OBJ_PROGRAM4) -o $@ $(LDFLAGS)	

$(TARGET5): $(OBJ_PROGRAM5)
	$(CC) $(OBJ_PROGRAM5) -o $@ $(LDFLAGS)

$(TARGET6): $(OBJ_PROGRAM6)
	$(CC) $(OBJ_PROGRAM6) -o $@ $(LDFLAGS)
	
$(TARGET7): $(OBJ_PROGRAM7)
	$(CC) $(OBJ_PROGRAM7) -o $@ $(LDFLAGS)
	
$(TARGET8): $(OBJ_PROGRAM8)
	$(CC) $(OBJ_PROGRAM8) -o $@ $(LDFLAGS)

$(TARGET9): $(OBJ_PROGRAM9)
	$(CC) $(OBJ_PROGRAM9) -o $@ $(LDFLAGS)

$(TARGET10): $(OBJ_PROGRAM10)
	$(CC) $(OBJ_PROGRAM10) -o $@ $(LDFLAGS)

$(TARGET11): $(OBJ_PROGRAM11)
	$(CC) $(OBJ_PROGRAM11) -o $@ $(LDFLAGS)

$(TARGET12): $(OBJ_PROGRAM12)
	$(CC) $(OBJ_PROGRAM12) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *_processor
