SRCDIR 		:= src
BUILDDIR 	:= build
TARGET 		:= bin/mfg-test
  
SRCEXT 		:= cc
SOURCES 	:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS 	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS 		:= -g -Wall -std=c++11
LIB 		:= -pthread -L lib
INC 		:= -I include

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CXX) $(CFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
tester:
	$(CXX) $(CFLAGS) test/tester.cx $(INC) $(LIB) -o bin/tester

.PHONY: clean

-include -f Makefile.local
