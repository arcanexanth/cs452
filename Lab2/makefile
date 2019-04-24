# Compiler for prime sieve
# Made by Adam Ray & George Hamlin
#
# #flags

CFLAGS = -g

#target

TARGET = sieve
FILES = *.cpp *.h

all: $(TARGET)


$(TARGET): $(FILES)
	$(CXX) $(FILES) $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
