# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <pthread.h>

static void receiveAll(int sockfd) {
	char buffer[4096];
	ssize_t received;

	while ((received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[received] = '\0';
		printf("%s", buffer);
		fflush(stdout);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Error: Invalid number of argumants\n");
		printf("Usage: ./clnt <port_number>\n");
		return 1;
	}

	int sockfd, portno;
	struct sockaddr_in serv_addr;
	char menu_buffer[4096];
	char choice[32];
	ssize_t received;

	portno = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket");
		return 1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		perror("connect");
		close(sockfd);
		return 1;
	}

	received = recv(sockfd, menu_buffer, sizeof(menu_buffer) - 1, 0);
	if (received > 0) {
		menu_buffer[received] = '\0';
		printf("%s", menu_buffer);
		fflush(stdout);
	}
	else if (received == -1) {
		perror("recv");
		close(sockfd);
		return 1;
	}

	printf("Your choice: ");
	fflush(stdout);
	if (fgets(choice, sizeof(choice), stdin) == NULL) {
		close(sockfd);
		return 1;
	}
	choice[strcspn(choice, "\r\n")] = '\0';

	if (send(sockfd, choice, strlen(choice), 0) == -1) {
		perror("send");
		close(sockfd);
		return 1;
	}

	receiveAll(sockfd);
	close(sockfd);
	return 0;
}
