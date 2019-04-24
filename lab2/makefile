all: head node

head: head.cpp uwecSocket.h
	g++ head.cpp uwecSocket.cpp -o head

node: node.cpp uwecSocket.h
	g++ node.cpp uwecSocket.cpp -o head

clean:
	rm -f *~
	rm -f *.o
	rm -f head
	rm -f node
