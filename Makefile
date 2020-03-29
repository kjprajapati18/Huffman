all: Huffman.c avl.o minheap.o
	gcc Huffman.c avl.o minheap.o

buildTest: Huffman.c avl.o minheap.o test.txt
	make clean
	make all
	./a.out -b test.txt

avl.o: avl.c
	gcc -c avl.c

minheap.o: minheap.c
	gcc -c minheap.c

a.out:
	make all

clean:
	rm avl.o
	rm minheap.o
	rm a.out