LIB_PATH = /usr/lib
LIBS = -lboost_system -lboost_mpi -lboost_serialization
CC = mpic++ 
CFLAGS = -std=c++11 -Wall -Ofast -march=native \
-pedantic \
-m64 \
-msse4.2 \
-mfpmath=sse \
-flto 
-funsafe-loop-optimizations \
-DNDEBUG \
-funroll-loops \
-floop-strip-mine \
-ffast-math \
-fipa-matrix-reorg \
-floop-interchange \
-floop-parallelize-all \
-floop-block \
-ftree-loop-if-convert \
-ftree-loop-if-convert-stores \
-ftree-loop-distribution \
-ftree-vectorize \
--param omega-hash-table-size=10031 \
$(LINK)
LINK = -L $(LIB_PATH)
OBJS = matrix.o matrix_multiply.o matrix_multiply_parallel.o
EXEC = mm

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

matrix: matrix.o matrix_multiply.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(EXEC)

parallel: matrix_multiply_parallel.o matrix.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(EXEC)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -c

clean:
	rm *.o $(EXEC)