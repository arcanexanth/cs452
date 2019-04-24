CC = g++
CPPFLAGS = -Wall -g -std=c++0x
SOURCES = src/Process.cpp src/main.cpp src/Time_Queue.cpp src/Scheduler.cpp src/MFQS.cpp src/RTS.cpp src/WHS.cpp src/RBTree.cpp
OBJECTS = $(SOURCES:.cpp=.o)
LFLAGS = 
EXECUTABLE = main

all: $(EXECUTABLE)
	
%.o: %.c
	$(CC) -o $@ -c $< $(CPPFLAGS)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(CPPFLAGS) $(LFLAGS)
	
clean:
	@echo "Cleaning..."
	@rm -f $(OBJECTS) $(EXECUTABLE)
	@echo "Done!"
	
tar:
	@echo "Tarring files to turn in:"
	@tar -cvf zutterlp_tjadenad_lab3.tar src/ Makefile
	
.PHONY: clean tar thing1 thing2
