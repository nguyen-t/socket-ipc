BIN = ./bin
JSFILES = ./js_files
CFILES = ./c_files
CFLAGS = -lpthread -O2

all: $(CFILES)/server.c
	gcc $(CFILES)/server.c -o $(BIN)/server $(CFLAGS)

run:
	$(BIN)/server

node:
	node $(JSFILES)

clean:
	rm -r $(BIN)/*
