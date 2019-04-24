# Compiler for shell
# Make by Adam Ray & George Hamlin
#
# #flags

CFLAGS = -g -lfl

#target

TARGET = run
FILES = lex.yy.c myshell.c

all: $(TARGET)

lex.yy.c: lex.c
	flex lex.c

$(TARGET): $(FILES)
	$(CC) $(FILES) $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
