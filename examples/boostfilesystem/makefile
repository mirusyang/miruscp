# BEGIN makefile Makefile
# GNUmakefile
################################################################################

CC = gcc
XX = g++
CFLAGS = -Wall -O2 -g -D_DEBUG -c
CSUFFIX = -std=c++0x
INCLUDE = -I. -I"E:/SDK_losses/boost/boost_1_54_0"
LIBS = -L"E:/SDK_losses/boost/boost_1_54_0/stage/lib" -lboost_filesystem-mgw47-mt-d-1_54.a

SRCS = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
TARGET = bfs

############################################################################
.PHONY:all clean
all:bfs

# bfs
$(TARGET):$(OBJS)
	$(XX) -o $@ $^ $(LIBS)

# make rule(s)
%.o:%.cpp
	$(XX) $(CFLAGS) -c $^ -o $@ $(INCLUDE) $(CSUFFIX)

############################################################################
# .PHONY:clean
clean:
	-rm -f $(OBJS) $(TARGET) *~
# END makefile

