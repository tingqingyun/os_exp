all:receiver sender

sender:sender.c
	gcc sender.c -o sender -lpthread
receiver:receiver.c
	gcc receiver.c -o receiver -lpthread
clean:
	rm receiver
	rm sender
