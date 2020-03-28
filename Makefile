all: Huffman.c avl.o minheap.o
	gcc Huffman.c avl.o minheap.o

avl.o: avl.c
	gcc -c avl.c

minheap.o: minheap.c
	gcc -c minheap.c

a.out:
	make all

buildTest: a.out test.txt
	./a.out -b test.txt

clean:
	rm avl.o
	rm minheap.o
	rm a.out