CC = mpicxx
SRCDIR = src
BUILDDIR = build
TARGET = bin/bitonicSort

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS = -std=c++1y \
-g \
-Wall \
-Ofast \
-march=native \
-pedantic \
-m64 \
-msse4.2 \
-mfpmath=sse \
-flto \
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
-ftree-vectorize

LIB = -lpapi
INC = -I include

$(TARGET): $(OBJS)
	$(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean

