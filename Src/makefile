CXX = g++-11
CXXFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O3 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
		   -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
		   -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
		   -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
		   -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self \
		   -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
		   -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		   -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand \
		   -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix \
		   -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
		   -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
		   -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie \
		   -fPIE -Werror=vla -D NDEBUG

HOME = $(shell pwd)
CXXFLAGS += -I $(HOME)

PROGRAMDIR = build/bin
TARGET = hashTable
OBJECTDIR = build
DOXYFILE = Others/Doxyfile

MAIN_CPP			= main.cpp
MAIN_OBJ			= $(MAIN_CPP:%.cpp=$(OBJECTDIR)/%.o)

TESTING_DIR			= Testing
TESTING_CPP 		= TestCorrectness.cpp
TESTING_OBJ 	 	= $(TESTING_CPP:%.cpp=$(OBJECTDIR)/%.o)

HASH_TABLE_DIR     	= HashTable
HASH_TABLE_CPP	  	= HashTable.cpp HashTableElem.cpp
HASH_TABLE_OBJ	  	= $(HASH_TABLE_CPP:%.cpp=$(OBJECTDIR)/%.o)

HASH_TABLE_LIST_DIR = HashTable/HashTableList
HASH_TABLE_LIST_CPP = HashTableList.cpp
HASH_TABLE_LIST_OBJ = $(HASH_TABLE_LIST_CPP:%.cpp=$(OBJECTDIR)/%.o)

HASHES_DIR      	= Hashes
HASHES_CPP			= Hashes.cpp
HASHES_OBJ      	= $(HASHES_CPP:%.cpp=$(OBJECTDIR)/%.o)

COMMON_DIR     		= Common
COMMON_CPP	 		= Log.cpp
COMMON_OBJ     		= $(COMMON_CPP:%.cpp=$(OBJECTDIR)/%.o)

.PHONY: all docs clean install
	
all: $(TARGET)

$(TARGET): $(HASH_TABLE_OBJ) $(HASHES_OBJ) $(COMMON_OBJ) $(HASH_TABLE_LIST_OBJ) \
					     $(MAIN_OBJ) $(TESTING_OBJ)
	$(CXX) $^ -o $(TARGET) $(CXXFLAGS)

$(OBJECTDIR)/%.o : $(HASH_TABLE_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(HASHES_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(COMMON_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(HASH_TABLE_LIST_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(TESTING_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 


$(OBJECTDIR)/%.o : %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

docs: 
	doxygen $(DOXYFILE)

clean:
	rm -rf $(OBJECTDIR)/*.o

buildDirs:
	mkdir $(OBJECTDIR)
	mkdir $(PROGRAMDIR)