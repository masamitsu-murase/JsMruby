VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_TEENY = 1
PLUGIN_NAME = libjsmruby-plugin.so
CHROME = chrome
CRX_NAME = JsMruby
PACK_VERSION = $(VERSION_MAJOR)_$(VERSION_MINOR)_$(VERSION_TEENY)
ifeq ($(shell which ruby),)
	PACK_SUFFIX = $(PACK_VERSION)
else
	PACK_SUFFIX = $(PACK_VERSION)-$(shell ruby -rrbconfig -e 'print RbConfig::CONFIG["target"]')
endif
CRX_NAME = JsMruby
CRX = $(CRX_NAME)_$(PACK_SUFFIX)

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
	rm -rf $(wildcard $(CRX)*)
	mkdir $(CRX)
	cp -r $(filter-out %manifest.json %.dll, $(wildcard chrome_extension/*)) $(CRX)/
	cp npapi/src/$(PLUGIN_NAME) $(CRX)/
	sed -e "s/[^\"]\+\.dll/$(PLUGIN_NAME)/" chrome_extension/manifest.json > $(CRX)/manifest.json
	$(CHROME) --pack-extension=$(CRX)
	rm -rf $(CRX)
