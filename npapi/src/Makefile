TARGET = libjsmruby.so
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXX = g++
LDSHARED = $(CXX) -shared
INC_DIRS = $(INCDIRS) ../include ../sdk
CXXFLAGS = $(patsubst %,-I%,$(INC_DIRS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(LDSHARED) $(OBJS) $(LIBMRUBY) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
