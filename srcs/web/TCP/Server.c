# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <pthread.h>

int main(int argc, char *argv[]) {

	pthread_t th;

	if (argc != 2) {
		printf("Error: Invalid number of argumants\n");
		printf("Usage: ./srvr <port_number>\n");
		return 1;
	}
	int sockfd, newsockfd, portno, clilen, n;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	// newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	// the senario

	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		pthread_create(&th, NULL, NULL /* add function later */, &newsockfd);
	}

	close(newsockfd);
	close(sockfd);
}
