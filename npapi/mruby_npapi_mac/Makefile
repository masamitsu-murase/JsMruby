
CC = gcc
CXX = g++
ifeq ($(ARCH),i386)
  CFLAGS = -Wall -O3 -fPIC -DXP_MACOSX=1 -arch i386
  LFLAGS = -bundle -arch i386
else
  CFLAGS = -Wall -O3 -fPIC -DXP_MACOSX=1 -arch x86_64
  LFLAGS = -bundle -arch x86_64
endif
INCLUDE = -I../include -I../sdk -I../../../mruby/include
LIBS = ../../../mruby/lib/libmruby.a
OBJS = mrb_js.o np_entry.o npp_gate.o mrb_js_object.o npn_gate.o plugin.o


all: npjsmrby_bundle


npjsmrby_bundle: npjsmrby_bundle_dir npjsmrby
	cp -f npjsmrby npjsmrby.bundle/Contents/MacOS/
	cp -f Info.plist npjsmrby.bundle/Contents/
	cp -f -R English.lproj npjsmrby.bundle/Contents/Resources/

npjsmrby_bundle_dir:
	mkdir -p npjsmrby.bundle
	mkdir -p npjsmrby.bundle/Contents/Resources
	mkdir -p npjsmrby.bundle/Contents/MacOS

npjsmrby: $(OBJS)
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS) : %.o : ../src/%.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) -c $< -o $@


clean:
	rm -f -r $(OBJS) npjsmrby.bundle
