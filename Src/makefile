CXX = g++-11
ASM = nasm
CXXFLAGS = -O3 -mavx2 -D NDEBUG #-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \
		   
HOME = $(shell pwd)
CXXFLAGS += -I $(HOME)

PROGRAMDIR = build/bin
TARGET = hashTable
OBJECTDIR = build
DOXYFILE = Others/Doxyfile

MAIN_CPP			= main.cpp
MAIN_OBJ			= $(MAIN_CPP:%.cpp=$(OBJECTDIR)/%.o)

UNIT_TEST_DIR	    = UnitTests
UNIT_TEST_CPP 		= UnitTestHashMap.cpp
UNIT_TEST_OBJ 	 	= $(UNIT_TEST_CPP:%.cpp=$(OBJECTDIR)/%.o)

TEST_PERFORMANCE_DIR  = TestPerformance
TEST_PERFORMANCE_CPP  = TestPerformance.cpp
TEST_PERFORMANCE_ASM  = GetTimeStampCounter.s
TEST_PERFORMANCE_OBJ  = $(TEST_PERFORMANCE_CPP:%.cpp=$(OBJECTDIR)/%.o)
TEST_PERFORMANCE_OBJ +=	$(TEST_PERFORMANCE_ASM:%.s=$(OBJECTDIR)/%.o)

HASH_TABLE_DIR     	= HashTable
HASH_TABLE_CPP	  	= HashTable.cpp HashTableElem.cpp
HASH_TABLE_OBJ	  	= $(HASH_TABLE_CPP:%.cpp=$(OBJECTDIR)/%.o)

HASH_TABLE_LIST_DIR  = HashTable/HashTableList
HASH_TABLE_LIST_CPP  = HashTableList.cpp
HASH_TABLE_LIST_ASM  = HashTableListAsm.s
HASH_TABLE_LIST_OBJ  = $(HASH_TABLE_LIST_CPP:%.cpp=$(OBJECTDIR)/%.o)
HASH_TABLE_LIST_OBJ += $(HASH_TABLE_LIST_ASM:%.s=$(OBJECTDIR)/%.o)

HASHES_DIR      	= Hashes
HASHES_CPP			= Hashes.cpp
HASHES_OBJ      	= $(HASHES_CPP:%.cpp=$(OBJECTDIR)/%.o)

COMMON_DIR     		= Common
COMMON_CPP	 		= Log.cpp
COMMON_OBJ     		= $(COMMON_CPP:%.cpp=$(OBJECTDIR)/%.o)

FAST_IO_DIR			= FastIO
FAST_IO_CPP			= InputOutput.cpp StringFuncs.cpp
FAST_IO_OBJ			= $(FAST_IO_CPP:%.cpp=$(OBJECTDIR)/%.o)

.PHONY: all docs clean install
	
all: $(TARGET)

$(TARGET): $(HASH_TABLE_LIST_OBJ) $(HASH_TABLE_OBJ) $(HASHES_OBJ) $(COMMON_OBJ) \
					     $(MAIN_OBJ) $(UNIT_TEST_OBJ) $(TEST_PERFORMANCE_OBJ) $(FAST_IO_OBJ)
	$(CXX) $^ -o $(TARGET) $(CXXFLAGS)

$(OBJECTDIR)/%.o : $(HASH_TABLE_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(HASHES_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(FAST_IO_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(COMMON_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(HASH_TABLE_LIST_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(HASH_TABLE_LIST_DIR)/%.s
	$(ASM) -f elf64 $< -o $@

$(OBJECTDIR)/%.o : $(UNIT_TEST_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(TEST_PERFORMANCE_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

$(OBJECTDIR)/%.o : $(TEST_PERFORMANCE_DIR)/%.s
	$(ASM) -f elf64 $< -o $@

$(OBJECTDIR)/%.o : %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) 

docs: 
	doxygen $(DOXYFILE)

clean:
	rm -rf $(OBJECTDIR)/*.o

buildDirs:
	mkdir $(OBJECTDIR)
	mkdir $(PROGRAMDIR)