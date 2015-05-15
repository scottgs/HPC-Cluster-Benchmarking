# Compiler for MPI programs
CC = mpicc

# Source, Build, BIN, Target directory paths
SRCDIR = src
BUILDDIR = build
BINDIR = bin
TARGET = bin/mm

SRCEXT = c

# Include all cpp files to use as sources and feed them to use as objects.
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS = -std=c99 \
-Wall -Ofast -march=native \
-pedantic \
-m64 \
-msse4.2 \
-mfpmath=sse \
-flto \
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

# Normal libs to always include
LIBS = -lpapi

INC = -I include

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR) $(TARGET) && $(RM) -r $(BINDIR)

.PHONY: clean


