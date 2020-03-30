all: Huffman.c avl.o minheap.o codebookWriter.o inputHandler.o
	gcc Huffman.c avl.o minheap.o codebookWriter.o inputHandler.o

buildTest: Huffman.c avl.o minheap.o codebookWriter.o inputHandler.o test.txt
	make clean
	make all
	./a.out -b test.txt

avl.o: avl.c
	gcc -c avl.c

minheap.o: minheap.c
	gcc -c minheap.c

codebookWriter.o: codebookWriter.c
	gcc -c codebookWriter.c

inputHandler.o: inputHandler.c
	gcc -c inputHandler.c

a.out:
	make all

clean:
	rm avl.o
	rm minheap.o
	rm inputHandler.o
	rm codebookWriter.o
	rm a.out