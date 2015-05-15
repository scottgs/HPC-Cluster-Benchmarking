SUBDIRS = Bitonic-Merge-Sort Matrix-Multiplication

all: subdirs
	@echo "Running Make for all of the subdirectories."

subdirs:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done


