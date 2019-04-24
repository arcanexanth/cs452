# Compiler for schedulers 
# Made by Adam Ray & George Hamlin
#
# #flags

CFLAGS = -g -std=c++11

#target

TARGET = run 
FILES = *.cpp *.h

all: $(TARGET)


$(TARGET): $(FILES)
	$(CXX) $(FILES) $(CFLAGS) -o $(TARGET)

clean:
	$(RM) $(TARGET)
