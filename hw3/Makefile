.phony: clean
CFLAGS = gcc

all: hw3_client hw3 hw3_tester

hw3_client: hw3_client.c 
	$(CFLAGS) -o hw3_client hw3_client.c 

hw3_tester: hw3_tester.c 
	$(CFLAGS) -o hw3_tester hw3_tester.c -lpthread

hw3: hw3.c 
	$(CFLAGS) -o hw3 hw3.c -lpthread

clean:
	rm -f hw3_client hw3 hw3_tester