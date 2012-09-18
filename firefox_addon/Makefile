PLUGIN_NAME = libjsmruby-plugin.so
PLUGIN_SRC = ../npapi/src/$(PLUGIN_NAME)
PLUGIN = plugins/$(PLUGIN_NAME)
XPI_NAME = jsmruby_$(PACK_SUFFIX).xpi
XPI_DIR = ..
XPI = $(XPI_DIR)/$(XPI_NAME)

.PHONY: all clean

all: $(XPI)

clean:
	rm -f $(PLUGIN) $(XPI)

$(PLUGIN): $(PLUGIN_SRC)
	cp $< $@

$(XPI): $(PLUGIN) $(filter-out . %.dll ./Makefile %~, $(shell find .))
	zip $@ $^
