VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_TEENY = 1
VERS = VERSION_MAJOR VERSION_MINOR VERSION_TEENY
VARS = $(foreach var, $(VERS), $(var)=$($(var)))
.PHONY: all npapi xpi

all: npapi

npapi:
	$(MAKE) -C npapi

xpi: npapi
	$(MAKE) -C firefox_addon $(VARS)
