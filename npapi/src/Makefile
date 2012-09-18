TARGET = $(PLUGIN_NAME)
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXX = g++
LDSHARED = $(CXX) -shared
LIBMRUBY = -lmruby
INC_DIRS = $(INCDIRS) ../include ../sdk
CXXFLAGS = $(patsubst %,-I%,$(INC_DIRS))

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(LDSHARED) $(OBJS) $(LIBMRUBY) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
