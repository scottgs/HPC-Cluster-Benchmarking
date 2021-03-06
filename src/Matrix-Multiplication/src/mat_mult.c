#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h> // for srand and timing the program

#include <papi.h>
#include "papi_wrapper.h"

#define MIN(x, y) ( (x < y) ? x : y )
#define IDX_INTO_ARRAY(x, y, dimX) ( x + (y * dimX) )
#define DATA_VALUE(A, row, col) ( A->data[IDX_INTO_ARRAY(col, row, A->numCols)] )

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

/**
 * TODO: Tiling, HPC improvements, run in blocks or strides
 */
void matrixMultiply(Matrix * partialResult, const Matrix * m1, const Matrix * m2, int beginRow, int endRow) {
    const unsigned int m1Rows = m1->numRows,
                       m2Cols = m2->numCols,
                       partialRows = partialResult->numRows;

    for (unsigned int k = 0; k < m1Rows; ++k) {
        for (unsigned int i = 0; i < partialRows; ++i) {
            const int rowOffsetted = i + beginRow;
            const double r = DATA_VALUE(m1, rowOffsetted, k);
            for (unsigned int j = 0; j < m2Cols; ++j) {
                DATA_VALUE(partialResult, i, j) += r * DATA_VALUE(m2, k, j);
            }
        }
    }
}

Matrix * matrixMultiplyHelper(const Matrix * A, const Matrix * B, int procRank, int numProcs) {
    // Start the collecting of statistics for non-master nodes.
    papiEvents * events;
    events = startEvents();

     /* Discussion on splitting work when it's not an even split between # of processors: 
     * http://stackoverflow.com/questions/9269399/sending-blocks-of-2d-array-in-c-using-mpi?rq=1
     * 
     * If need to send different numbers of items to each process? Then you need a generalized version of scatter, MPI_Scatterv().
     * - lets you specify the counts for each processor, and displacements -- where in the global array that piece of data starts.
     */
    
    // Split up the work (i.e. number of rows) for each process
    const int work = A->numRows / numProcs;
    const int beginRow = procRank * work;
    const int endRow = MIN( (procRank+1) * work, A->numRows );

    // Allocate space for the result portion
    Matrix * portion = malloc(sizeof(Matrix));
    portion->numRows = endRow - beginRow;
    // [aRows, aCols] x [bRows, bCols] => result[aRows, bCols] iff aCols == bRows
    portion->numCols = B->numCols;
    portion->data = calloc(portion->numRows * portion->numCols, sizeof(double));

    matrixMultiply(portion, A, B, portion->numRows, portion->numCols);

    stopEvents(events);
    //print summary for non-master
    printf("Process %d had %lld L1 misses, %lld L2 misses, %lld loads\n", procRank, events->values[0], events->values[1], events->values[2]);

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

    const int numElements = partialResult->numRows * partialResult->numCols;
    MPI_Gather(partialResult->data, numElements, MPI_DOUBLE,
               finalResult->data, numElements, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
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

void check_command_line(int argc, char* argv[]) {
    if (argc < 4) {
        printf("Too few arguments!\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned int i = 0; i < argc; ++i) {
        if (argv[i] == NULL) {
            printf("One of the arguments is not valid.\n");
            exit(EXIT_FAILURE);
        }
    }
}

// TODO: use CSV file and write relative timing statistics to file
// TODO: generate script to constantly run this program on larger and larger input sizes
int main(int argc, char *argv[]) {
    int processRank, numProcs;
    clock_t start_time, end_time;
    start_time = clock();

    check_command_line(argc, argv);

    // Initialize the MPI execution environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank); // rank for each process
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs); // let the MPI world know how many processes it can deal with in its group

    // Set up PAPI library environment
    initializePapi();

    // Create random matrices to multiply
    srand((unsigned int)time(NULL));
    Matrix * A = generate_rand_matrix(atoi(argv[1]), atoi(argv[2]));
    Matrix * B = generate_rand_matrix(atoi(argv[2]), atoi(argv[3]));

    // Delegate work.
    Matrix * portion = matrixMultiplyHelper(A, B, processRank, numProcs);

    // Collect and merge results.
    const int productSize = A->numRows * B->numCols;
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
        
    // Clean up all MPI state.
    MPI_Finalize();

    // Free resources used by PAPI
    shutdown();

    // Free up the memory associated with the matrices.
    cleanup(A);
    cleanup(B);
    cleanup(portion);

    end_time = clock();
    const double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("To do the MPI Matrix multiplication for process %d took %lf seconds.\n", processRank, elapsed_time);

    return EXIT_SUCCESS;
}