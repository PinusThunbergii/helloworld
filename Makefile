CC := gcc
CXX := g++
OPTFLAG := -O0
DBGFLAG := -g
CXXFLAGS := -c $(OPTFLAG) $(DBGFLAG) -pipe -march=native -std=c++14
BUILD_PATH := 
SRCLIST := src/tga/tgaimage.cpp src/tga/tgaimage.h
OUTPUT_NAME := tgaimage.o

#all: main.o src/tga/tgaimage.o
#	@echo "LOL"
#	@echo $^
#	gcc $^ -o OUTPUT_NAME 
#tgaimage.o: $(SRCLIST)
#	$(CXX) $(CXXFLAGS) $^ -o $@

SRC_DIR := src/ 
INC_DIR := inc/
BUILD_DIR := build/

SRC_FILE_EXTENSION := *.c *.cpp
INC_FILE_EXTENSION := *.h *.hpp

#SEARCH_WILDCARD_SRC := $(addsuffix *.cpp, $(SRC_DIR)) 
#SEARCH_WILDCARD_SRC += $(addsuffix *.c, $(SRC_DIR)) 
#SEARCH_WILDCARD_INC := $(addsuffix $(INC_FILE_EXTENSION), $(INC_DIR)) 

NUMBER_OF_SRC_FILE_EXTENSION := $(words $(SRC_FILE_EXTENSION))
NUMBER_OF_INC_FILE_EXTENSION := $(words $(INC_FILE_EXTENSION))

#SEARCH_WILDCARD_SRC := $(foreach EXTENSION, $(SRC_FILE_EXTENSION), $(addsuffix, $(EXTENSION), $(SRC_DIR)))
ITER = $(addsuffix $(EXTENSION), $(SRC_DIR)) 
SEARCH_WILDCARD_SRC := $(foreach EXTENSION, $(SRC_FILE_EXTENSION), $(ITER))

ITER = $(addsuffix $(EXTENSION), $(INC_DIR)) 
SEARCH_WILDCARD_INC := $(foreach EXTENSION, $(INC_FILE_EXTENSION), $(ITER))

.PHONY: test
test:
	@echo $(SEARCH_WILDCARD_SRC)
	@echo $(SEARCH_WILDCARD_INC)
	@echo $(NUMBER_OF_SRC_FILE_EXTENSION)

.cpp.o:
	$(CXX) $(CXXFLAGS) $^ 

.PHONY: clean

clean:
	rm -f *.o 
	rm -f *.elf
