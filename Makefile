SRCDIR 		:= src
BUILDDIR 	:= build
LIBDIR	 	:= lib
TARGET 		:= bin/mfg-test
  
SRCEXT 		:= cc
SOURCES 	:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS 	:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS 		:= -g -Wall
CXXFLAGS	:= -g -Wall -std=c++11
LIB 		:= -pthread -L lib
INC 		:= -I include

CANSRCEXT	:= c
CANSRCS		:= $(shell find $(LIBDIR) -type f -name *.$(CANSRCEXT))
CANOBJS		:= $(patsubst $(LIBDIR)/%,$(BUILDDIR)/%,$(CANSRCS:.$(CANSRCEXT)=.o))

$(TARGET): $(OBJECTS) $(CANOBJS)
	@echo " Linking..."
	@echo " $(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/%.o: $(LIBDIR)/%.$(CANSRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
tester:
	$(CXX) $(CFLAGS) test/tester.cx $(INC) $(LIB) -o bin/tester

.PHONY: clean

-include -f Makefile.local
