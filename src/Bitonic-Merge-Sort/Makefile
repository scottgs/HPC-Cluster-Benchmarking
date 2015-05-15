CC = mpicxx
SRCDIR = src
BUILDDIR = build
TARGET = bin/bitonicSort

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS = \
-std=c++11 \
-march=native \
-Ofast \
-DNDEBUG \
-mveclibabi=svml \
-mfpmath=sse \
-msse4.2 \
-flto \
-ftracer \
-ffast-math \
-fmerge-all-constants \
-fno-signaling-nans \
-fno-unsafe-math-optimizations \
-fivopts \
-fipa-matrix-reorg \
-fvect-cost-model \
-floop-flatten \
-funroll-loops \
-floop-strip-mine \
-floop-parallelize-all \
-floop-block \
-ftree-vectorize \
-ftree-loop-optimize \
-ftree-loop-if-convert \
-ftree-loop-if-convert-stores \
-ftree-loop-distribution \
-ftree-loop-distribute-patterns \
-faggressive-loop-optimizations \
-funsafe-loop-optimizations \
-fvariable-expansion-in-unroller  \
-fbranch-target-load-optimize \
-m64 \
-BOOST_CPPFLAGS \
-BOOST_LDFLAGS \
-D_DEBUG

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

