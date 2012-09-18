TARGET = libjsmruby.so
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
AR = ar
CXX = g++
INC_DIRS = $(INCDIRS) ../include ../sdk
CXXFLAGS = $(patsubst %,-I%,$(INC_DIRS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(AR) r $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
