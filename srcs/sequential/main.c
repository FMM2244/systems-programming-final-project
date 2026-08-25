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

	res->nb_rows = (rand() % (50 - 3 + 1)) + 3;
	res->nb_columns = (rand() % (50 - 3 + 1)) + 3;

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

mtrx_t *multiply(mtrx_t *A, mtrx_t *B) {

}

int avarage(mtrx_t *m) {
	long res = 0;
	for (int i = 0; i < m->nb_rows; i++)
		for (int j = 0; j < m->nb_columns; j++)
			res += m->mtrx[i][j];
	return (int)(res / (m->nb_rows * m->nb_columns));
}

mtrx_t *transposition(mtrx_t *A, mtrx_t *B) {

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
	mtrx_t *C = multiply(&A, &B);
	struct timeval multiEnd;
	gettimeofday(&multiEnd, NULL);
	printf("Time taken to perform Multiplication: %ld\n", (long)(multiEnd.tv_sec - multiStart.tv_sec + (multiEnd.tv_usec - multiStart.tv_usec) * 1000000));
	printf("Result Matrix:\n\n");
	printMatrix(*C);

	// 2. Matrix Transposition
	printf("\n==================================================================\n");
	printf("\nPerforming Matrix Transposition on Matrix (A) & (B)\n");
	struct timeval tranStart;
	gettimeofday(&tranStart, NULL);
	mtrx_t *D = transposition(&A, &B);
	struct timeval tranEnd;
	gettimeofday(&tranEnd, NULL);
	printf("Time taken to perform Multiplication: %ld\n", (long)(tranEnd.tv_sec - tranStart.tv_sec + (tranEnd.tv_usec - tranStart.tv_usec) * 1000000));
	printf("Result Matrix:\n\n");
	printMatrix(*D);

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

	return 0;
}
