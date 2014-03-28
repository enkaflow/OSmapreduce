CC = gcc
CCFLAGS = -Wall -lpthread -g
mapred: tokenizer.h sorted-list.h sorted-list.c mapred.h mapred.c main.c
	$(CC) $(CCFLAGS) tokenizer.h sorted-list.h sorted-list.c mapred.h mapred.c main.c -o mapred
clean: 
	rm -f mapred *.o