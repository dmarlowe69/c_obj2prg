OBJS=obj2prg.o
OBJSCR=obj2prgcr.o
CFLAGS=-Wall -O3
BIN=C:\BIN
all: obj2prg obj2prgcr

obj2prg: $(OBJS)
	$(CC) -o obj2prg $(OBJS)
	
obj2prgcr: $(OBJSCR)
	$(CC) -o obj2prgcr $(OBJSCR)	

install:
	cp obj2prg.exe $(BIN)
	cp obj2prgcr.exe $(BIN)
clean:
	rm -f $(OBJS) obj2prg obj2prgcr

