C = gcc
LFLAG = -lwiringPi
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
	$(CC) $(OBJDIR)/*.o -o $@ $(LFLAG)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(LFLAG)

clean:
	-rm -f $(OBJDIR)/*.o $(EXE)

run: 
	./$(BINDIR)/bin