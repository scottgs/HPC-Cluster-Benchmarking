SUBDIRS = Simple_Cache_Analysis

all: subdirs
	@echo "Building the examples."

subdirs:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done


