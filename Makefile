# # Executavel
# BINFOLDER := bin/
# # .h
# INCFOLDER := inc/
# # .c
# SRCFOLDER := src/
# # .o
# OBJFOLDER := obj/
# CC := gcc
# CFLAGS := -W -Wall -pedantic
# SRCFILES := $(wildcard src/*.c)
# all: $(SRCFILES:src/%.c=obj/%.o)
# 	$(CC) $(CFLAGS) obj/*.o -o bin/prog
# obj/%.o: src/%.c
# 	$(CC) $(CFLAGS) -c $< -o $@ -I./inc
# run: bin/prog
# 	bin/prog
# .PHONY: clean
# clean:
# 	rm -rf obj/*
# 	rm -rf bin/*

C = gcc
BLDDIR = .
INCDIR = $(BLDDIR)/inc
SRCDIR = $(BLDDIR)/src
OBJDIR = $(BLDDIR)/obj
BINDIR = $(BLDDIR)/bin
CFLAGS = -c -Wall -I$(INCDIR)
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
EXE = bin/bin

all: clean $(EXE) 
    
$(EXE): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJDIR)/*.o -o $@ 

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJDIR)/*.o $(EXE)

run: 
	./$(BINDIR)/bin