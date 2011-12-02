CC=gcc
#OPT= -g
SRC=phpserialize*.c
OUT=phpserialize.so

all:
	$(CC) $(OPT) -c -fPIC $(SRC)
	$(CC) -shared -fPIC -o $(OUT) *.o

clean:
	rm -f $(OUT) *.o test

test:
	$(CC) $(OPT) -o test *.c