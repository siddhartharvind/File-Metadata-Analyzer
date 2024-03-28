CXX      = g++
CXXFLAGS =  -std=c++20
CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Werror
CXXFLAGS += -O0
CXXFLAGS += -g3

# MinGW, MSYS on Windows do not have
# the UBSan, ASan libraries by default.
ifneq ($(OS), Windows_NT)
	CXXFLAGS += -fsanitize=undefined,address
endif


# Program name
OUTPUT_NAME = filemeta

ifeq ($(OS), Windows_NT)
	OUTPUT_FILE = .\\$(OUTPUT_NAME)
else
	OUTPUT_FILE = ./$(OUTPUT_NAME)
endif


# First/default recipe
.PHONY: all
all: $(OUTPUT_FILE)



main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o


$(OUTPUT_FILE): main.o
	$(CXX) $(CXXFLAGS) main.o -o $(OUTPUT_FILE)


.PHONY: clean
clean:
	-$(RM) $(OUTPUT_FILE)
	-$(RM) *.o
