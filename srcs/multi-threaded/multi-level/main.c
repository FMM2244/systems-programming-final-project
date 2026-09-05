/***
 * 
 * 
 * HEADER COMMENT
 * 
 * 
 */

/** =============================================================
 * INCLUDED LIBRARIES HERE
 ** ========================================================== */

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
# include <pthread.h>

/** =============================================================
 * DEFINED STRUCTS HERE
 ** =============================================================
 */

/**
 * a struct that holds a 2 dimentional array its rows and columns
 */
typedef struct matrix {
	unsigned int nb_rows;
	unsigned int nb_columns;
	int **mtrx;
}	mtrx_t;

typedef struct sub {
	mtrx_t *dest;
	mtrx_t *src;
	int row_index;
}	sub_t;

/** =============================================================
 * GLOBAL VARIABLES HERE
 ** =============================================================
 */

mtrx_t A;
mtrx_t B;

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

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
void waitForAllThreads(pthread_t *th, int size) {

	for (int i = 0; i < size; i++)
		pthread_join(th[i], NULL);
}

/**
 * prints the contents of a 2 dimentional matrix on the standerd output
 */
void printMatrix(mtrx_t m, int fd) {
	for (int i = 0; i < m.nb_rows; i++) {
		for (int j = 0; j < m.nb_columns; j++)
			dprintf(fd, "%d\t", m.mtrx[i][j]);
		dprintf(fd, "\n");
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
 * 
 */
void *multiplySubRoutine(void *data) {

	sub_t *con = (sub_t *)data;

	for (int j = 0; j < con->dest->nb_columns; j++) {
		con->dest->mtrx[con->row_index][j] = 0;
		// multiply row of A by column of B
		for (int k = 0; k < A.nb_columns; k++) {
			con->dest->mtrx[con->row_index][j] += A.mtrx[con->row_index][k] * B.mtrx[k][j];
		}
	}
}

void *transposeSubRoutine(void *data) {

	sub_t *con = (sub_t *)data;

	for (int j = 0; j < con->dest->nb_columns; j++)
		con->dest->mtrx[con->row_index][j] = con->src->mtrx[j][con->row_index];

	return NULL;
}

/**
 * multiplies matrix A by B and stores the result in matrix C
 */
int multiply(mtrx_t *C) {

	C->nb_rows = A.nb_rows;
	C->nb_columns = B.nb_columns;

	if (A.nb_columns != B.nb_rows)
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

	pthread_t th[C->nb_rows];
	sub_t con[C->nb_rows];

	for (int i = 0; i < C->nb_rows; i++) {
		con[i].dest = C;
		con[i].row_index = i;
		pthread_create(&th[i], NULL, multiplySubRoutine, &con[i]);
	}
	
	waitForAllThreads(th, C->nb_rows);

	return 0;
}

/**
 * Sub-routine for calculateing the avarage
 */
void *avarageSubRoutine(void *data) {

	sub_t *con = (sub_t *)data;
	int *res = malloc(sizeof(*res));
	if (res == NULL)
		return NULL;
	*res = 0;

	for (unsigned int j = 0; j < con->dest->nb_columns; j++)
		*res += con->dest->mtrx[con->row_index][j];
	return res;	
}

/**
 * calculate the avarage of matrix m
 */
int avarage(char mtrx) {

	long res = 0;

	if (mtrx == 'A') {
		pthread_t th[A.nb_rows];
		sub_t con[A.nb_rows];
		int arr[A.nb_rows];

		for (int i = 0; i < A.nb_rows; i++) {
			con[i].dest = &A;
			con[i].row_index = i;
			pthread_create(&th[i], NULL, avarageSubRoutine, &con[i]);
		}

		for (int i = 0; i < A.nb_rows; i++) {
			void *thread_result;
			pthread_join(th[i], &thread_result);
			arr[i] = *(int *)thread_result;
			free(thread_result);
		}

		for (int i = 0; i < A.nb_rows; i++)
			res += arr[i];

		return (int)(res / (A.nb_rows * A.nb_columns));
	}
	else {
		pthread_t th[B.nb_rows];
		sub_t con[B.nb_rows];
		int arr[B.nb_rows];

		for (int i = 0; i < B.nb_rows; i++) {
			con[i].dest = &B;
			con[i].row_index = i;
			pthread_create(&th[i], NULL, avarageSubRoutine, &con[i]);
		}

		for (int i = 0; i < B.nb_rows; i++) {
			void *thread_result;
			pthread_join(th[i], &thread_result);
			arr[i] = *(int *)thread_result;
			free(thread_result);
		}

		for (int i = 0; i < B.nb_rows; i++)
			res += arr[i];

		return (int)(res / (B.nb_rows * B.nb_columns));
	}

	return res;
}

/**
 * transposis matrix A and stores the output in matrix B
 */
int transposition(char mtrx, mtrx_t *C) {
	if (mtrx == 'A') {
		C->nb_rows = A.nb_columns;
		C->nb_columns = A.nb_rows;
	}
	else {
		C->nb_rows = B.nb_columns;
		C->nb_columns = B.nb_rows;
	}

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

	pthread_t th[C->nb_rows];
	sub_t con[C->nb_rows];
	mtrx_t *src = (mtrx == 'A') ? &A : &B;

	for (int i = 0; i < C->nb_rows; i++) {
		con[i].dest = C;
		con[i].src = src;
		con[i].row_index = i;
		pthread_create(&th[i], NULL, transposeSubRoutine, &con[i]);
	}

	waitForAllThreads(th, C->nb_rows);
	
	return 0;
}

/**
 * prints the output text files on the terminal by executing the cat command
 */
void printResults() {
	printf("==================================================================\n");
	printf("Matrix Multiplication on (A) & (B) result: \n");
	system("cat a_b_multiplication_result.txt");
	printf("==================================================================\n");
	printf("Matrix Transposition on (A) result: \n");
	system("cat a_transpose_result.txt");
	printf("==================================================================\n");
	printf("Matrix Transposition on (B) result: \n");
	system("cat b_transpose_result.txt");
	printf("==================================================================\n");
	printf("Matrix Avarage of (A) result: \n");
	system("cat a_avarage_result.txt");
	printf("==================================================================\n");
	printf("Matrix Avarage of (B) result: \n");
	system("cat b_avarage_result.txt");
}

/**
 * multiplication routine
 */
void *multiplyControler(void *data) {

	pthread_mutex_lock(&print_lock);
	printf("Performing Matrix Multiplication on Matrices (A) & (B)\n");
	pthread_mutex_unlock(&print_lock);	int fd = open("a_b_multiplication_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
	if (fd == -1) {
		pthread_mutex_lock(&print_lock);
		printf("Error: can't open file \"a_b_multiplication_result.txt\"\n");
		pthread_mutex_unlock(&print_lock);
		return NULL;
	}
	struct timeval multiStart;
	gettimeofday(&multiStart, NULL);
	mtrx_t C;
	int flag = multiply(&C);
	struct timeval multiEnd;
	gettimeofday(&multiEnd, NULL);
	dprintf(fd, "Time taken: %ld us\n", (long)((multiEnd.tv_sec - multiStart.tv_sec) * 1000000 + multiEnd.tv_usec - multiStart.tv_usec));
	if (!flag) {
		dprintf(fd, "Result Matrix:\n\n");
		printMatrix(C, fd);
		freeMatrix(&C);
	}
	else
		dprintf(fd, "Error: Can't Multiply Matrices\n");
	close(fd);
}

/**
 * transposition routine
 */
void *transposeControler(void *data) {

	char mtrx = *(char *)data;
	int fd = 0;
	if (mtrx == 'A') {
		pthread_mutex_lock(&print_lock);
		printf("Performing Matrix Transposition on Matrix (A)\n");
		pthread_mutex_unlock(&print_lock);
		fd = open("a_transpose_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			pthread_mutex_lock(&print_lock);
			printf("Error: can't open file \"a_transpose_result.txt\"\n");
			pthread_mutex_unlock(&print_lock);
			return NULL;
		}
	}
	else {
		pthread_mutex_lock(&print_lock);
		printf("Performing Matrix Transposition on Matrix (B)\n");
		pthread_mutex_unlock(&print_lock);
		fd = open("b_transpose_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			pthread_mutex_lock(&print_lock);
			printf("Error: can't open file \"b_transpose_result.txt\"\n");
			pthread_mutex_unlock(&print_lock);
			return NULL;
		}
	}
	struct timeval tranStart;
	gettimeofday(&tranStart, NULL);
	mtrx_t D;
	bzero(&D, sizeof(mtrx_t *));
	int flag = transposition(mtrx, &D);
	struct timeval tranEnd;
	gettimeofday(&tranEnd, NULL);
	dprintf(fd, "Time taken: %ld us\n", (long)((tranEnd.tv_sec - tranStart.tv_sec) * 1000000 + tranEnd.tv_usec - tranStart.tv_usec));
	if (!flag) {
		dprintf(fd, "Result Matrix:\n\n");
		printMatrix(D, fd);
		freeMatrix(&D);
	}
	else
		printf("Error: Can't Transpose Matrix\n");
	close(fd);
}

/**
 * avarage routine
 */
void *avarageControler(void *data) {

	char mtrx = *(char *)data;
	int fd = 0;
	if (mtrx == 'A') {
		pthread_mutex_lock(&print_lock);
		printf("Finding Matrix Avarage of Matrix (A)\n");
		pthread_mutex_unlock(&print_lock);
		fd = open("a_avarage_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			pthread_mutex_lock(&print_lock);
			printf("Error: can't open file \"a_avarage_result.txt\"\n");
			pthread_mutex_unlock(&print_lock);
			return NULL;
		}
	}
	else {
		pthread_mutex_lock(&print_lock);
		printf("Finding Matrix Avarage of Matrix (B)\n");
		pthread_mutex_unlock(&print_lock);
		fd = open("b_avarage_result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1) {
			pthread_mutex_lock(&print_lock);
			printf("Error: can't open file \"b_avarage_result.txt\"\n");
			pthread_mutex_unlock(&print_lock);
			return NULL;
		}
	}
	struct timeval avgStart;
	gettimeofday(&avgStart, NULL);
	int E = avarage(mtrx);
	struct timeval avgEnd;
	gettimeofday(&avgEnd, NULL);
	dprintf(fd, "Time taken: %ld us\n", (long)((avgEnd.tv_sec - avgStart.tv_sec) * 1000000 + avgEnd.tv_usec - avgStart.tv_usec));
	dprintf(fd, "Matrix (A) Avarage = %d\n", E);
	close(fd);
}

/**
 * main function
 * everything starts here ...
 */
int main(int ac, char **av) {

	srand(time(NULL));

	pthread_t ths[5];

	bzero(&A, sizeof(mtrx_t *));
	bzero(&B, sizeof(mtrx_t *));
	generateRandomMatrix(&A);
	generateRandomMatrix(&B);

	// just to check
	// comment this part if you don't the output to be too long
	printf("Matrix (A) rows: %u, columns: %u\n", A.nb_rows, A.nb_columns);
	printf("==================================================================\n");
	printMatrix(A, 0);
	printf("Matrix (B) rows: %u, columns: %u\n", B.nb_rows, B.nb_columns);
	printf("==================================================================\n");
	printMatrix(B, 0);

	printf("==================================================================\n");
	printf("performing operations...\nplease wait momentarally :)\n");
	printf("==================================================================\n");

	// Start Overall_Timer

	struct timeval start;
	gettimeofday(&start, NULL);

	// Matrix Multiplication
	pthread_create(&ths[0], NULL, multiplyControler, NULL);

	// Matrix Transposition
	char mtrx_A = 'A';
	char mtrx_B = 'B';
	pthread_create(&ths[1], NULL, transposeControler, &mtrx_A);
	pthread_create(&ths[2], NULL, transposeControler, &mtrx_B);

	// Matrix Average
	pthread_create(&ths[3], NULL, avarageControler, &mtrx_A);
	pthread_create(&ths[4], NULL, avarageControler, &mtrx_B);

	// wait for all children to finish
	waitForAllThreads(ths, 5);

	// print results
	printResults();

	// Stop Overall_Timer
	struct timeval end;
	gettimeofday(&end, NULL);
	printf("Total time taken: %ld us\n", (long)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec));

	freeMatrix(&A);
	freeMatrix(&B);

	return 0;
}
