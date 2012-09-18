.PHONY: all npapi xpi

all: npapi

npapi:
	$(MAKE) -C npapi

xpi: npapi
	$(MAKE) -C firefox_addon
