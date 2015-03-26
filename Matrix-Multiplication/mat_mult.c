#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h> // for srand and timing the program

#define MIN(x, y) ( (x < y) ? x : y )
#define LIN(x, y, dimX) ( x + (y * dimX) )
#define DATA_VALUE(A, row, col) ( A->data[LIN(col, row, A->numCols)] )

// #define _DEBUG

typedef struct {
    double * data;
    unsigned int numRows, 
                 numCols;
} Matrix;

void cleanup(Matrix * mat) {
    free(mat->data);
    free(mat);
}

void naiveMatrixMultiply(Matrix * C, const Matrix * A, const Matrix * B) {
    for (unsigned int i = 0; i < C->numRows; ++i) {
        for (unsigned int col = 0; col < B->numCols; ++col) {
            for (unsigned int j = 0; j < A->numCols; ++j) {
                DATA_VALUE(C, i, col) += DATA_VALUE(A, i, j) * DATA_VALUE(B, j, col);
            }
        }
    }
}

void matrix_multiply(Matrix * partialResult, const Matrix * m1, const Matrix * m2, int beginRow, int endRow) {
    const unsigned int m1Rows = m1->numRows,
                       m2Cols = m2->numCols,
                       partialRows = partialResult->numRows;

    for (unsigned int k = 0; k < m1Rows; ++k) {
        for (unsigned int i = 0; i < partialRows; ++i) {
            const int rowOffsetted = i + beginRow;
            double r = DATA_VALUE(m1, rowOffsetted, k);
            for (unsigned int j = 0; j < m2Cols; ++j) {
                DATA_VALUE(partialResult, i, j) += r * DATA_VALUE(m2, k, j);
            }
        }
    }
}

Matrix * matrixMultiplyHelper(const Matrix * A, const Matrix * B, int procRank, int numProcs) {
    // Split up the work (i.e. number of rows) for each process
    int work = A->numRows / numProcs;

    const int beginRow = procRank * work;
    const int endRow = MIN( (procRank+1) * work, A->numRows );

    // Allocate space for the result portion
    Matrix * portion = malloc(sizeof(Matrix));
    portion->numRows = endRow - beginRow;
    // [aRows, aCols] x [bRows, bCols] => result[aRows, bCols] iff aCols == bRows
    portion->numCols = B->numCols;
    portion->data = calloc(portion->numRows * portion->numCols, sizeof(double));

    matrix_multiply(portion, A, B, portion->numRows, portion->numCols);

    return portion;
}

void collect(Matrix * partialResult, int procRank, int productSize) {
    Matrix * finalResult;
    // Master node allocates enough space for the receive buffer
    if (0 == procRank) {
        finalResult = malloc(sizeof(Matrix));
        finalResult->data = malloc(productSize * sizeof(double));
        // [aRows, aCols] x [bRows, bCols] => result[aRows, bCols] iff aCols == bRows
        finalResult->numCols = partialResult->numCols;
        finalResult->numRows = productSize / partialResult->numCols;
    }

    int numElements = partialResult->numRows * partialResult->numCols;
    MPI_Gather(partialResult->data, numElements, MPI_DOUBLE,
               finalResult->data, numElements, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    #ifdef _DEBUG
        printf("%f\n", finalResult[0]);
    #endif
}

Matrix* generate_rand_matrix(int numRows, int numCols) {
    // Allocate and initialize the matrix
    Matrix * mat = malloc(sizeof(Matrix));
    mat->numRows = numRows;
    mat->numCols = numCols;
    mat->data = malloc(numRows * numCols * sizeof(double));

    const unsigned int upperLimit = 100; 
    const unsigned int numElements = numRows * numCols;
    for (unsigned int i = 0; i < numElements; ++i) {
        // Generate a random double value in the range [0..upperLimit]
        mat->data[i] = (double)rand() / (double)(RAND_MAX / upperLimit);
    }

    return mat;
}

int main(int argc, char *argv[]) {
    int processRank, numProcs;
    clock_t start_time, end_time;
    start_time = clock();

    // Initialize the MPI execution environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // rank for each process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs); // let the MPI world know how many processes it can deal with in its group

    // Create random matrices to multiply
    srand((unsigned int)time(NULL));
    Matrix * A = generate_rand_matrix(atoi(argv[1]), atoi(argv[2]));
    Matrix * B = generate_rand_matrix(atoi(argv[2]), atoi(argv[3]));

    // Delegate work.
    Matrix * portion = matrixMultiplyHelper(A, B, processRank, numProcs);

    // Collect and merge results.
    int productSize = A->numRows * B->numCols;
    collect(portion, processRank, productSize);

    #ifdef _DEBUG
        Matrix * testMatrix = malloc(sizeof(Matrix));
        testMatrix->data = malloc(productSize * sizeof(double));
        testMatrix->numRows = A->numRows;
        testMatrix->numCols = B->numCols;

        // Do the multiplication.
        naiveMatrixMultiply(testMatrix, A, B);

        // Check each element is the same
        for (unsigned int i = 0; i < testMatrix->numRows; ++i) {
            for (unsigned int j = 0; j < testMatrix->numCols; ++j) {
                if (DATA_VALUE(testMatrix, i, j) != DATA_VALUE(result, i, j))
                    return EXIT_FAILURE;
            }
        }
        cleanup(testMatrix);
    #endif

    // Free up the memory associated with the matrices.
    cleanup(A);
    cleanup(B);
    cleanup(portion);
    
    // Clean up all MPI state.
    MPI_Finalize();

    end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("To do the MPI Matrix multiplication for this process took %lf seconds.\n", elapsed_time);

    return EXIT_SUCCESS;
}