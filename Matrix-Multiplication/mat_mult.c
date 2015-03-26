#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h> // for srand and timing the program
#include <papi.h>

#define MIN(x, y) ( (x < y) ? x : y )
#define IDX_INTO_ARRAY(x, y, dimX) ( x + (y * dimX) ) // since using matrix data as 1D array
#define DATA_VALUE(A, row, col) ( A->data[IDX_INTO_ARRAY(col, row, A->numCols)] )

#define NUM_EVENTS 1
#define ERROR_RETURN(errorCode) { fprintf(stderr, "Error %d %s:line %d: \n", errorCode,__FILE__,__LINE__);  exit(errorCode); }

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
            const double r = DATA_VALUE(m1, rowOffsetted, k);
            for (unsigned int j = 0; j < m2Cols; ++j) {
                DATA_VALUE(partialResult, i, j) += r * DATA_VALUE(m2, k, j);
            }
        }
    }
}

Matrix * matrixMultiplyHelper(const Matrix * A, const Matrix * B, int procRank, int numProcs) {
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

int main(int argc, char *argv[]) {
    // int retVal;

    // Must be initialized to PAPI_NULL before calling PAPI_create_event
    // int eventSet = PAPI_NULL;

    // char errString[PAPI_MAX_STR_LEN];
    // long long values[NUM_EVENTS];

    /****************************************************************************
    * This part initializes the library and compares the version number of the  *
    * header file, to the version of the library. If if these don't match, then *
    * it is likely that PAPI won't work correctly. If there is an error, retval *
    * keeps track of the version number.                                        *
    ****************************************************************************/
  
    // if( (retVal = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT ) {
    //     fprintf(stderr, "Error: %s\n", errString);
    //     exit(EXIT_FAILURE);
    // }

    // // Creating event set
    // if ( (retVal = PAPI_create_eventset(&eventSet)) != PAPI_OK) {
    //     ERROR_RETURN(retVal);
    // }

    // // Add the array of events PAPI_TOT_INS and PAPI_TOT_CYC to the eventSet
    // if ( (retVal = PAPI_add_event(eventSet, PAPI_L1_DCM)) != PAPI_OK ) {
    //     ERROR_RETURN(retVal);
    // }

    // // Start counting
    // if ( (retVal = PAPI_start(eventSet)) != PAPI_OK ) {
    //     ERROR_RETURN(retVal);
    // }


    /****************************************************************************
    *           Start the MPI variable declaration and matrix multiplication
    * ***************************************************************************/
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

    // /* Stop counting, this reads from the counter as well as stop it. */
    // if ( (retVal=PAPI_stop(eventSet,values)) != PAPI_OK) {
    //     ERROR_RETURN(retVal);
    // }

    // const unsigned long totalOps = A->numRows * B->numCols * B->numCols + A->numRows + B->numCols;
    // const double missRatio = values[0] / totalOps;
    // printf("Miss ratio is %f\n", missRatio);

    // if ( (retVal=PAPI_remove_event(eventSet,PAPI_L1_DCM)) !=PAPI_OK ) {
    //     ERROR_RETURN(retVal);
    // }

    // // Free all PAPI memory and data structures, Event set must be empty.
    // if ( (retVal=PAPI_destroy_eventset(&eventSet)) != PAPI_OK ) {
    //     ERROR_RETURN(retVal);
    // }

    // Free the resources used by PAPI
    // PAPI_shutdown();

    // Free up the memory associated with the matrices.
    cleanup(A);
    cleanup(B);
    cleanup(portion);
    
    // Clean up all MPI state.
    MPI_Finalize();

    end_time = clock();
    const double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("To do the MPI Matrix multiplication for this process took %lf seconds.\n", elapsed_time);

    return EXIT_SUCCESS;
}