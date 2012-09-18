TARGET = libjsmruby-plugin.so
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXX = g++
LDSHARED = $(CXX) -shared
LIBMRUBY = -lmruby
INC_DIRS = $(INCDIRS) ../include ../sdk
CXXFLAGS = $(patsubst %,-I%,$(INC_DIRS)) -DXP_UNIX

all: $(TARGET)
$(TARGET): $(OBJS)
	$(LDSHARED) $(OBJS) $(LIBMRUBY) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
