all: Huffman.c avl.o minheap.o codebookWriter.o inputHandler.o
	gcc -o fileCompressor Huffman.c avl.o minheap.o codebookWriter.o inputHandler.o 

buildTest: Huffman.c avl.o minheap.o codebookWriter.o inputHandler.o test.txt
	make clean
	make all
	./fileCompressor -b test.txt
	diff HuffmanCodebook HuffmanCodebook2 | wc

avl.o: avl.c
	gcc -c avl.c

minheap.o: minheap.c
	gcc -c minheap.c

codebookWriter.o: codebookWriter.c
	gcc -c codebookWriter.c

inputHandler.o: inputHandler.c
	gcc -c inputHandler.c

recurseTest: fileCompressor
	./fileCompressor -R -b test2
	./fileCompressor -R -c test2 HuffmanCodebook
	./fileCompressor -R -d test2 HuffmanCodebook
	diff test2/test4.txt test2/test4.txt.hcz.txt
	diff test2/test5.txt test2/test5.txt.hcz.txt
	diff test2/test10/test.txt test2/test10/test.txt.hcz.txt
	diff test2/test10/test2.txt test2/test10/test2.txt.hcz.txt

cleanText: fileCompressor
	rm test2/test4.txt.hcz*
	rm test2/test5.txt.hcz*
	rm test2/test10/test.txt.hcz*
	rm test2/test10/test2.txt.hcz*

memcheck: fileCompressor
	valgrind --leak-check=full ./fileCompressor -R -b test2
	valgrind --leak-check=full ./fileCompressor -R -c test2 HuffmanCodebook
	valgrind --leak-check=full ./fileCompressor -R -d test2	HuffmanCodebook

fileCompressor:
	make all

clean:
	rm avl.o
	rm minheap.o
	rm inputHandler.o
	rm codebookWriter.o
	rm fileCompressor