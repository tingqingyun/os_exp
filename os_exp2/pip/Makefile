all:father testsize 
father:father.c son.c header.h
	gcc father.c son.c -o father -pthread
testsize:testsize.c
	gcc testsize.c -o testsize
clean:
	rm father
	rm testsize
