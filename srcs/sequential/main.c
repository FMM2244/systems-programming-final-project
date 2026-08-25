# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <sys/types.h>

/**
 * 
 */
typedef struct matrix {
	unsigned int nb_rows;
	unsigned int nb_columns;
	int **mtrx;
}	mtrx_t;

mtrx_t GenerateRandomMatrix() {
	mtrx_t res;
}

int main(int ac, char **av) {
	// BEGIN
    // // Dynamic generation
    // Set rows, cols randomly
    mtrx_t A = GenerateRandomMatrix();
    mtrx_t B = GenerateRandomMatrix();
    
    // Start Overall_Timer
    
    // // 1. Matrix Multiplication
    // Start Mul_Timer
    // Matrix C = Multiply(A, B)
    // Stop Mul_Timer
    // Print "Multiplication Time: ", Mul_Timer
    
    // // 2. Matrix Transposition
    // Start Trans_Timer
    // Matrix T = Transpose(A)
    // Stop Trans_Timer
    // Print "Transposition Time: ", Trans_Timer
    
    // // 3. Matrix Average
    // Start Avg_Timer
    // Float average = CalculateAverage(A)
    // Stop Avg_Timer
    // Print "Average Time: ", Avg_Timer
    
    // Stop Overall_Timer
    // Print "Total Sequential Execution Time: ", Overall_Timer
	// END

	return 0;
}
