all: bankingClient bankingServer
bankingServer: bankingServer.c bankingServer.h
	gcc -pthread -o bankingServer bankingServer.c
bankingClient: bankingClient bankingClient.h
	gcc -pthread -o bankingClient bankingClient.c
clean:
	rm -r bankingClient bankingServer
