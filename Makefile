VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_TEENY = 1
PLUGIN_NAME = libjsmruby-plugin.so
CHROME = chrome

VERS = VERSION_MAJOR VERSION_MINOR VERSION_TEENY
VARS = $(foreach var, $(VERS) PLUGIN_NAME, $(var)=$($(var)))

.PHONY: all clean npapi xpi crx

all: npapi

clean:
	$(MAKE) clean -C npapi $(VARS)
	$(MAKE) clean -C firefox_addon $(VARS)

npapi:
	$(MAKE) -C npapi $(VARS)

xpi: npapi
	$(MAKE) -C firefox_addon $(VARS)

crx: npapi npapi/src/$(PLUGIN_NAME)
	rm -rf $(wildcard JsMruby*)
	mkdir JsMruby
	cp -r $(filter-out %manifest.json %.dll, $(wildcard chrome_extension/*)) JsMruby/
	cp npapi/src/$(PLUGIN_NAME) JsMruby/
	sed -e "s/[^\"]\+\.dll/$(PLUGIN_NAME)/" chrome_extension/manifest.json > JsMruby/manifest.json
	$(CHROME) --pack-extension=JsMruby
	rm -rf JsMruby
