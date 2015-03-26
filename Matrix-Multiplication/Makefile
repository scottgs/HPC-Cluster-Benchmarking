CC = mpicc
OBJS = mat_mult.o
EXEC = mm
CFLAGS = -std=c99 \
-Wall -Ofast -march=native \
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

PAPI_LINK=-lpapi

all: $(OBJS)
	$(CC) $(CFLAGS) $^ $(PAPI_LINK) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) $< $(PAPI_LINK) -c

clean:
	rm *.o $(EXEC)