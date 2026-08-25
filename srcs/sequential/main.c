# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <time.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <sys/types.h>

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
	for (unsigned int i = 0; i < m->nb_rows; i++)
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

	if (A->nb_columns != B->nb_rows) {
		printf("Error: Can't Multiply Matrices\n");
		return 1;
	}

	C->mtrx = malloc(A->nb_rows * sizeof(int *));
	if (C->mtrx == NULL) {
		printf("Error: Can't Multiply Matrices\n");
		return 1;
	}

	for (int i = 0; i < B->nb_columns; i++) {
		C->mtrx[i] = malloc(B->nb_columns * sizeof(int));

		if (C->mtrx[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(C->mtrx[j]);
			free(C->mtrx);
			return 1;
		}
	}

	for (int i = 0; i < C->nb_rows; i++)
		for (int j = 0; j < C->nb_columns; j++) {
			C->mtrx[i][j] = 0;
			// multiply row of A by column of B
			for (int k = 0; k < A->nb_columns; k++) {
				C->mtrx[i][j] += A->mtrx[i][k] * B->mtrx[k][j];
			}
		}

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
		printf("Error: Can't Traspose Matrix\n");
		return 1;
	}

	for (int i = 0; i < B->nb_rows; i++) {
		B->mtrx[i] = malloc(B->nb_columns * sizeof(int));

		if (B->mtrx[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(B->mtrx[j]);
			free(B->mtrx);
			printf("Error: Can't Traspose Matrix\n");
			return 1;
		}
	}

	for (int i = 0; i < B->nb_rows; i++)
		for (int j = 0; j < B->nb_columns; j++)
			B->mtrx[i][j] = A->mtrx[j][i];
	
	return 0;
}

/**
 * main function
 * everything starts here ...
 */
int main(int ac, char **av) {

	srand(time(NULL));

	mtrx_t A;
	mtrx_t B;

	generateRandomMatrix(&A);
	generateRandomMatrix(&B);

	// just to check
	printf("Matrix (A) rows: %u, columns: %u\n", A.nb_rows, A.nb_columns);
	printf("==================================================================\n");
	printMatrix(A);
	printf("Matrix (B) rows: %u, columns: %u\n", B.nb_rows, B.nb_columns);
	printf("==================================================================\n");
	printMatrix(B);

	// Start Overall_Timer

	struct timeval start;
	gettimeofday(&start, NULL);

	// 1. Matrix Multiplication
	printf("\n==================================================================\n");
	printf("\nPerforming Matrix Multiplication on Matrix (A) & (B)\n");
	struct timeval multiStart;
	gettimeofday(&multiStart, NULL);
	mtrx_t C;
	int flag = multiply(&A, &B, &C);
	struct timeval multiEnd;
	gettimeofday(&multiEnd, NULL);
	printf("Time taken to perform Multiplication: %ld\n", (long)(multiEnd.tv_sec - multiStart.tv_sec + (multiEnd.tv_usec - multiStart.tv_usec) * 1000000));
	if (!flag) {
		printf("Result Matrix:\n\n");
		printMatrix(C);
	}

	// 2. Matrix Transposition
	printf("\n==================================================================\n");
	printf("\nPerforming Matrix Transposition on Matrix (A)\n");
	struct timeval tranStart;
	gettimeofday(&tranStart, NULL);
	mtrx_t D;
	flag = transposition(&A, &D);
	struct timeval tranEnd;
	gettimeofday(&tranEnd, NULL);
	printf("Time taken to perform Transposition: %ld\n", (long)(tranEnd.tv_sec - tranStart.tv_sec + (tranEnd.tv_usec - tranStart.tv_usec) * 1000000));
	printf("Result Matrix:\n\n");
	if (!flag)
		printMatrix(D);
	freeMatrix(&D);

	printf("\n==================================================================\n");
	printf("\nPerforming Matrix Transposition on Matrix (B)\n");
	gettimeofday(&tranStart, NULL);
	flag = transposition(&B, &D);
	gettimeofday(&tranEnd, NULL);
	printf("Time taken to perform Transposition: %ld\n", (long)(tranEnd.tv_sec - tranStart.tv_sec + (tranEnd.tv_usec - tranStart.tv_usec) * 1000000));
	printf("Result Matrix:\n\n");
	if (!flag)
		printMatrix(D);

	// 3. Matrix Average
	printf("\n==================================================================\n");
	printf("\nPerforming Avarage Calculation on Matrix (A)\n");
	struct timeval avgStart;
	gettimeofday(&avgStart, NULL);
	int E = avarage(&A);
	struct timeval avgEnd;
	gettimeofday(&avgEnd, NULL);
	printf("Time taken to calculate avarage: %ld\n", (long)(avgEnd.tv_sec - avgStart.tv_sec + (avgEnd.tv_usec - avgStart.tv_usec) * 1000000));
	printf("Matrix (A) Avarage = %d\n", E);
	
	printf("\n==================================================================\n");
	printf("\nPerforming Avarage Calculation on Matrix (B)\n");
	gettimeofday(&avgStart, NULL);
	E = avarage(&B);
	gettimeofday(&avgEnd, NULL);
	printf("Time taken to calculate avarage: %ld\n", (long)(avgEnd.tv_sec - avgStart.tv_sec + (avgEnd.tv_usec - avgStart.tv_usec) * 1000000));
	printf("Matrix (A) Avarage = %d\n", E);

	// Stop Overall_Timer

	struct timeval end;
	gettimeofday(&end, NULL);
	printf("Total time taken: %ld\n", (long)(end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) * 1000000));

	freeMatrix(&A);
	freeMatrix(&B);
	freeMatrix(&C);
	freeMatrix(&D);

	return 0;
}
