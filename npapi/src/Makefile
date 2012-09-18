TARGET = libjsmruby.so
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
AR = AR
CXX = g++
INCDIRS = ../include
CXXFLAGS = $(patsubst %,-I%,$(INCDIRS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(AR) r $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
