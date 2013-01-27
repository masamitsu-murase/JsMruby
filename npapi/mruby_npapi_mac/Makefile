
CC = gcc
CXX = g++
TARGET_I386 = npjsmrby.i386
TARGET_X64  = npjsmrby.x64
ifeq ($(ARCH),i386)
  CFLAGS = -Wall -O3 -fPIC -DXP_MACOSX=1 -arch i386
  LFLAGS = -bundle -arch i386
  TARGET = $(TARGET_I386)
else
  CFLAGS = -Wall -O3 -fPIC -DXP_MACOSX=1 -arch x86_64
  LFLAGS = -bundle -arch x86_64
  TARGET = $(TARGET_X64)
endif
INCLUDE = -I../include -I../sdk -I../../mruby/include
LIBS = ../../mruby/build/host/lib/libmruby.a
OBJS = mrb_js.o np_entry.o npp_gate.o mrb_js_object.o npn_gate.o plugin.o
TARGET_UNIV = npjsmrby

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS) : %.o : ../src/%.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -f -r $(OBJS) npjsmrby.plugin $(TARGET_UNIV)


.PHONY: univ
univ: $(TARGET_UNIV) npjsmrby_plugin

$(TARGET_UNIV): $(TARGET_I386) $(TARGET_X64)
	lipo -create $(TARGET_I386) $(TARGET_X64) -output npjsmrby

npjsmrby_plugin: npjsmrby_plugin_dir $(TARGET)
	cp -f npjsmrby npjsmrby.plugin/Contents/MacOS/
	cp -f Info.plist npjsmrby.plugin/Contents/
	cp -f -R English.lproj npjsmrby.plugin/Contents/Resources/

npjsmrby_plugin_dir:
	mkdir -p npjsmrby.plugin
	mkdir -p npjsmrby.plugin/Contents/Resources
	mkdir -p npjsmrby.plugin/Contents/MacOS

