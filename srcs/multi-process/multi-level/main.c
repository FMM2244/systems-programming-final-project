# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <time.h>
# include <errno.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <strings.h>

/**
 * a struct that holds a 2 dimentional array its rows and columns
 */
typedef struct matrix {
	unsigned int nb_rows;
	unsigned int nb_columns;
	int **mtrx;
}	mtrx_t;

/**
 * prints an error prompt based on the value passed as flag
 */
void handleError(int flag) {
	switch (flag) {
	case 0:
		perror("malloc");
		break;
	
	default:
		break;
	}
	exit(EXIT_FAILURE);
}

/**
 * waits for all child processes spawned by the parent to exit
 */
void waitForAllChildren(void) {
	pid_t pid;
	int status;

	while (1) {
		pid = waitpid(-1, &status, 0);
		if (pid == -1) {
			if (errno == EINTR)
				continue;
			if (errno == ECHILD)
				break;
			perror("waitpid");
			break;
		}
	}
}

/**
 * prints the contents of a 2 dimentional matrix on the standerd output
 */
void printMatrix(mtrx_t m) {
	for (int i = 0; i < m.nb_rows; i++) {
		for (int j = 0; j < m.nb_columns; j++)
			printf("%d\t", m.mtrx[i][j]);
		printf("\n");
	}
}

void freeMatrix(mtrx_t *m) {
	for (int i = 0; i < m->nb_rows; i++)
		free(m->mtrx[i]);
	free(m->mtrx);
}

/**
 * generates a matrix with random number of rows, columns and content
 */
void generateRandomMatrix(mtrx_t *res) {

	res->nb_rows = (rand() % (5 - 3 + 1)) + 3;
	res->nb_columns = (rand() % (5 - 3 + 1)) + 3;

	res->mtrx = malloc((res->nb_rows + 1) * sizeof(*res->mtrx));
	if (res->mtrx == NULL)
		handleError(0);
	res->mtrx[res->nb_rows] = NULL;

	for (int i = 0; i < res->nb_rows; i++) {
		res->mtrx[i] = malloc(res->nb_columns * sizeof(*res->mtrx[i]));

		if (res->mtrx[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(res->mtrx[j]);
			free(res->mtrx);
			handleError(0);
		}
		for (int j = 0; j < res->nb_columns; j++)
			res->mtrx[i][j] = (rand() % (50 - 3 + 1)) + 3;
	}
}

/**
 * multiplies matrix A by B and stores the result in matrix C
 */
int multiply(mtrx_t *A, mtrx_t *B, mtrx_t *C) {

	C->nb_rows = A->nb_rows;
	C->nb_columns = B->nb_columns;

	if (A->nb_columns != B->nb_rows)
		return 1;

	C->mtrx = malloc(C->nb_rows * sizeof(int *));
	if (C->mtrx == NULL)
		return 1;

	for (int i = 0; i < C->nb_rows; i++) {
		C->mtrx[i] = malloc(C->nb_columns * sizeof(int));

		if (C->mtrx[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(C->mtrx[j]);
			free(C->mtrx);
			return 1;
		}
	}

	int read_fds[C->nb_rows];

	for (int i = 0; i < C->nb_rows; i++) {
		int fds[2];
		if (pipe(fds) == -1) {
			perror("pipe");
			freeMatrix(A);
			freeMatrix(B);
			freeMatrix(C);
			waitForAllChildren();
			exit(EXIT_FAILURE);
		}
		int pid = fork();
		if (pid == 0) {
			close(fds[0]);
			dup2(fds[1], STDOUT_FILENO);
			for (int j = 0; j < C->nb_columns; j++) {
				C->mtrx[i][j] = 0;
				// multiply row of A by columns of B
				for (int k = 0; k < A->nb_columns; k++)
					C->mtrx[i][j] += A->mtrx[i][k] * B->mtrx[k][j];
			}
			write(fds[1], C->mtrx[i], sizeof(int) * C->nb_columns);
			freeMatrix(A);
			freeMatrix(B);
			freeMatrix(C);
			exit(EXIT_SUCCESS);
		}
		close(fds[1]);
		read_fds[i] = fds[0];
	}

	waitForAllChildren();

	for (int i = 0; i < C->nb_rows; i++)
		read(read_fds[i], C->mtrx[i], sizeof(int) * C->nb_columns);

	return 0;
}

/**
 * calculate the avarage of matrix m
 */
int avarage(mtrx_t *m) {
	long res = 0;

	for (int i = 0; i < m->nb_rows; i++)
		for (int j = 0; j < m->nb_columns; j++)
			res += m->mtrx[i][j];

	return (int)(res / (m->nb_rows * m->nb_columns));
}

/**
 * transposis matrix A and stores the output in matrix B
 */
int transposition(mtrx_t *A, mtrx_t *B) {
	B->nb_rows = A->nb_columns;
	B->nb_columns = A->nb_rows;

	B->mtrx = malloc(B->nb_rows * sizeof(int *));
	if (B->mtrx == NULL) {
		printf("Error: Can't Transpose Matrix\n");
		return 1;
	}

	for (int i = 0; i < B->nb_rows; i++) {
		B->mtrx[i] = malloc(B->nb_columns * sizeof(int));

		if (B->mtrx[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(B->mtrx[j]);
			free(B->mtrx);
			printf("Error: Can't Transpose Matrix\n");
			return 1;
		}
	}

	for (int i = 0; i < B->nb_rows; i++)
		for (int j = 0; j < B->nb_columns; j++)
			B->mtrx[i][j] = A->mtrx[j][i];
	
	return 0;
}

/**
 * prints the output text files on the terminal by executing the cat command
 */
void printResults() {
	int status;
	char *const multi_argv[] = {"/usr/bin/cat", "a_b_multiplication_result.txt", NULL};
	char *const a_transpose_argv[] = {"/usr/bin/cat", "a_transpose_result.txt", NULL};
	char *const b_transpose_argv[] = {"/usr/bin/cat", "b_transpose_result.txt", NULL};
	char *const a_avg_argv[] = {"/usr/bin/cat", "a_avarage_result.txt", NULL};
	char *const b_avg_argv[] = {"/usr/bin/cat", "b_avarage_result.txt", NULL};

	int pid = fork();
	if (pid == 0) {
		printf("==================================================================\n");
		printf("Matrix Multiplication on (A) & (B) result: \n");
		execvp(multi_argv[0], multi_argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	wait(&status);
	if (status)
		printf("Error: couldn't print file\ngo to a_b_multiplication_result.txt and check it yourself\n");

	pid = fork();
	if (pid == 0) {
		printf("==================================================================\n");
		printf("Matrix Transposition on (A) result: \n");
		execvp(a_transpose_argv[0], a_transpose_argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	wait(&status);
	if (status)
		printf("Error: couldn't print file\ngo to a_transpose_result.txt and check it yourself\n");

	pid = fork();
	if (pid == 0) {
		printf("==================================================================\n");
		printf("Matrix Transposition on (B) result: \n");
		execvp(b_transpose_argv[0], b_transpose_argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	wait(&status);
	if (status)
		printf("Error: couldn't print file\ngo to b_transpose_result.txt and check it yourself\n");

	pid = fork();
	if (pid == 0) {
		printf("==================================================================\n");
		printf("Matrix Avarage of (A) result: \n");
		execvp(a_avg_argv[0], a_avg_argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	wait(&status);
	if (status)
		printf("Error: couldn't print file\ngo to a_avarage_result.txt and check it yourself\n");

	pid = fork();
	if (pid == 0) {
		printf("==================================================================\n");
		printf("Matrix Avarage of (B) result: \n");
		execvp(b_avg_argv[0], b_avg_argv);
		perror("execvp");
		exit(EXIT_FAILURE);
	}
	wait(&status);
	if (status)
		printf("Error: couldn't print file\ngo to b_avarage_result.txt and check it yourself\n");
}

/**
 * main function
 * everything starts here ...
 */
int main(int ac, char **av) {

	srand(time(NULL));

	mtrx_t A = {0};
	mtrx_t B = {0};
	int pids[5];

	generateRandomMatrix(&A);
	generateRandomMatrix(&B);

	// just to check
	// comment this part if you don't the output to be too long
	printf("Matrix (A) rows: %u, columns: %u\n", A.nb_rows, A.nb_columns);
	printf("==================================================================\n");
	printMatrix(A);
	printf("Matrix (B) rows: %u, columns: %u\n", B.nb_rows, B.nb_columns);
	printf("==================================================================\n");
	printMatrix(B);

	printf("==================================================================\n");
	printf("performing operations...\nplease wait momentarally :)\n");
	// printf("==================================================================\n");

	// Start Overall_Timer
	struct timeval start, end;
	gettimeofday(&start, NULL);

	// Matrix Multiplication
	pids[0] = fork();
	if (pids[0] == 0) {
		// printf("\n==================================================================\n");
		int fd = open("a_b_multiplication_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			printf("Error: can't open file \"a_b_multiplication_result.txt\"\n");
			exit(EXIT_FAILURE);
		}
		struct timeval multiStart, multiEnd;
		gettimeofday(&multiStart, NULL);
		mtrx_t C;
		int flag = multiply(&A, &B, &C);
		gettimeofday(&multiEnd, NULL);
		dup2(fd, STDOUT_FILENO);
		close(fd);
		printf("Time taken: %ld us\n", (long)((multiEnd.tv_sec - multiStart.tv_sec) * 1000000 + multiEnd.tv_usec - multiStart.tv_usec));
		if (!flag) {
			printf("Result Matrix:\n\n");
			printMatrix(C);
			freeMatrix(&C);
		}
		else
			printf("Error: Can't Multiply Matrices\n");
		freeMatrix(&A);
		freeMatrix(&B);
		exit(EXIT_SUCCESS);
	}

	// Matrix Transposition
	pids[1] = fork();
	if (pids[1] == 0) {
		// printf("\n==================================================================\n");
		// printf("\nPerforming Matrix Transposition on Matrix (A)\n");
		int fd = open("a_transpose_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			printf("Error: can't open file \"a_transpose_result.txt\"\n");
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		struct timeval tranStart;
		gettimeofday(&tranStart, NULL);
		mtrx_t D = {0};
		int flag = transposition(&A, &D);
		struct timeval tranEnd;
		gettimeofday(&tranEnd, NULL);
		printf("Time taken: %ld us\n", (long)((tranEnd.tv_sec - tranStart.tv_sec) * 1000000 + tranEnd.tv_usec - tranStart.tv_usec));
		if (!flag) {
			printf("Result Matrix:\n\n");
			printMatrix(D);
			freeMatrix(&D);
		}
		exit(EXIT_SUCCESS);
	}

	pids[2] = fork();
	if (pids[2] == 0) {	
		// printf("\n==================================================================\n");
		// printf("\nPerforming Matrix Transposition on Matrix (B)\n");
		int fd = open("b_transpose_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			printf("Error: can't open file \"b_transpose_result.txt\"\n");
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		struct timeval tranStart;
		gettimeofday(&tranStart, NULL);
		mtrx_t D = {0};
		int flag = transposition(&B, &D);
		struct timeval tranEnd;
		gettimeofday(&tranEnd, NULL);
		printf("Time taken: %ld us\n", (long)((tranEnd.tv_sec - tranStart.tv_sec) * 1000000 + tranEnd.tv_usec - tranStart.tv_usec));
		if (!flag) {
			printf("Result Matrix:\n\n");
			printMatrix(D);
			freeMatrix(&D);
		}
		exit(EXIT_SUCCESS);
	}

	// Matrix Average
	pids[3] = fork();
	if (pids[3] == 0) {
		// printf("\n==================================================================\n");
		// printf("\nPerforming Avarage Calculation on Matrix (A)\n");
		int fd = open("a_avarage_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			printf("Error: can't open file \"a_avarage_result.txt\"\n");
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		struct timeval avgStart;
		gettimeofday(&avgStart, NULL);
		int E = avarage(&A);
		struct timeval avgEnd;
		gettimeofday(&avgEnd, NULL);
		printf("Time taken: %ld us\n", (long)((avgEnd.tv_sec - avgStart.tv_sec) * 1000000 + avgEnd.tv_usec - avgStart.tv_usec));
		printf("Matrix (A) Avarage = %d\n", E);
		exit(EXIT_SUCCESS);
	}

	pids[4] = fork();
	if (pids[4] == 0) {
		// printf("\n==================================================================\n");
		// printf("\nPerforming Avarage Calculation on Matrix (B)\n");
		int fd = open("b_avarage_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			printf("Error: can't open file \"b_avarage_result.txt\"\n");
			exit(EXIT_FAILURE);
		}
		dup2(fd, STDOUT_FILENO);
		close(fd);
		struct timeval avgStart;
		gettimeofday(&avgStart, NULL);
		int E = avarage(&B);
		struct timeval avgEnd;
		gettimeofday(&avgEnd, NULL);
		printf("Time taken: %ld us\n", (long)((avgEnd.tv_sec - avgStart.tv_sec) * 1000000 + avgEnd.tv_usec - avgStart.tv_usec));
		printf("Matrix (A) Avarage = %d\n", E);
		exit(EXIT_SUCCESS);
	}

	// wait for all children to finish
	waitForAllChildren();

	// print results
	printResults();

	// Stop Overall_Timer
	gettimeofday(&end, NULL);
	printf("Total time taken: %ld us\n", (long)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec));

	freeMatrix(&A);
	freeMatrix(&B);

	return 0;
}
