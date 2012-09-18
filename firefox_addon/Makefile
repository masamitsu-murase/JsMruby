PLUGIN_NAME = libjsmruby-plugin.so
PLUGIN_SRC = ../npapi/src/$(PLUGIN_NAME)
PLUGIN = plugins/$(PLUGIN_NAME)

.PHONY: all

all: ../jsmruby.xpi

$(PLUGIN): $(PLUGIN_SRC)
	cp $< $@

../jsmruby.xpi: $(PLUGIN) $(filter-out %.dll Makefile %~, $(wildcard *))
	zip -r $@ ./ -x *.dll Makefile *~
