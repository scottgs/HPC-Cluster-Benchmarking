# C++ Compiler
CC = g++

# Source, Build, BIN, Target directory paths
SRCDIR = src
BUILDDIR = build
BINDIR = bin
TARGET = bin/tests

SRCEXT = cpp

# Include all cpp files to use as sources and feed them to use as objects.
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

CFLAGS = -std=c++11 -march=native -Ofast

# Normal libs to always include
LIBS = -lpapi

INC = -I ../include

$(TARGET): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR) $(TARGET) && $(RM) -r $(BINDIR)

.PHONY: clean


