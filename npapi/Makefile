VARS = $(foreach var, PLUGIN_NAME, $(var)=$($(var)))

.PHONY: all clean

all:
	$(MAKE) -C src $(VARS)

clean:
	$(MAKE) $@ -C src $(VARS)
