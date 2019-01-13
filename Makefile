# Makefile for MPIC programs

BUILD = RELEASE

LINKCC = $(CC)

# Check if running with MS-MPI
ifeq ($(OS), Windows_NT)
 MPI_LIBRARY = msmpi
else
 MPI_LIBRARY = mpi
endif

# CXXFLAGS denotes flags for the C++ compiler
CC = gcc

ifeq (${BUILD}, DEBUG)
 BUILD_FLAGS = -std=c99 -O0 -g
else
 BUILD_FLAGS = -std=c99 -O3
endif

CXXFLAGS = ${BUILD_FLAGS} -Wall

SRCS = $(wildcard src/*.c)
EXEC := MonteCarlo.exe

all: $(EXEC)

MonteCarlo.exe:
	$(LINKCC) $(BUILD_FLAGS) -o $@ $(PROGS) $(SRCS) -l$(MPI_LIBRARY)

clean:
	rm -rf $(PROGS) $(EXEC)

.PHONY: all clean
