
CC = gcc
CXX = g++

CFLAGS = -Wall -O3 -fPIC -DXP_UNIX=1
LFLAGS = -shared
INCLUDE = -I../include -I../sdk -I../../mruby/include
LIBS = ../../mruby/build/host/lib/libmruby.a
OBJS = mrb_js.o np_entry.o npp_gate.o mrb_js_object.o npn_gate.o plugin.o
TARGET = npjsmrby.so


all: npjsmrby_linux


npjsmrby_linux: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS) : %.o : ../src/%.cpp
	$(CXX) $(INCLUDE) $(CFLAGS) -c $< -o $@


clean:
	rm -f -r $(OBJS)
