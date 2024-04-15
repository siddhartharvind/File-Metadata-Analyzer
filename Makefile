CXX      =  g++
CXXFLAGS =  -std=c++20
CXXFLAGS += -pedantic
CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Werror
CXXFLAGS += -O0
CXXFLAGS += -ggdb3

# MinGW, MSYS on Windows do not have
# the UBSan, ASan libraries by default.
ifneq ($(OS), Windows_NT)
	CXXFLAGS += -fsanitize=undefined
	CXXFLAGS += -fsanitize=address
endif


# Program name
OUTPUT_NAME = filemeta

ifeq ($(OS), Windows_NT)
	OUTPUT_FILE = bin\\$(OUTPUT_NAME)
else
	OUTPUT_FILE = bin/$(OUTPUT_NAME)
endif


# First/default recipe
.PHONY: all
all: $(OUTPUT_FILE)



obj/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o obj/main.o


$(OUTPUT_FILE): obj/main.o
	$(CXX) $(CXXFLAGS) obj/main.o -o $(OUTPUT_FILE)


.PHONY: clean
clean:
	-$(RM) $(OUTPUT_FILE)
	-$(RM) obj/*.o
