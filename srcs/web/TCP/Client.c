# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <pthread.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Error: Invalid number of argumants\n");
		printf("Usage: ./clnt <port_number>\n");
		return 1;
	}
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	// struct hostent *server;
	char buffer[256];
	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	//server = gethostbyname(argv[1]);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	//bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
	//senario

	

	close(sockfd);
}
