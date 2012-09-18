VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_TEENY = 1
PLUGIN_NAME = libjsmruby-plugin.so

VERS = VERSION_MAJOR VERSION_MINOR VERSION_TEENY
.PHONY: all npapi xpi
VARS = $(foreach var, $(VERS) PLUGIN_NAME, $(var)=$($(var)))

all: npapi

npapi:
	$(MAKE) -C npapi $(VARS)

xpi: npapi
	$(MAKE) -C firefox_addon $(VARS)
