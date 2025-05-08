all:sender1 sender2 receiver
sender1:sender1.c
	gcc sender1.c -o sender1 -pthread -lrt
sender2:sender2.c
	gcc sender2.c -o sender2 -pthread -lrt
receiver:receiver
	gcc receiver.c -o receiver -pthread -lrt
clean:
	rm sender1
	rm sender2
	rm receiver
