WX_CONFIG := wx-config
WX_CXXFLAGS := $(shell $(WX_CONFIG) --cxxflags)
WX_LIBS := $(shell $(WX_CONFIG) --libs)

SQLITE_LIB_DIR := /usr/lib/x86_64-linux-gnu
SQLITE_INCLUDE_DIR := /usr/include

LDFLAGS += -L$(SQLITE_LIB_DIR) -lsqlite3 $(WX_LIBS) 
WX_CXXFLAGS += -I$(SQLITE_INCLUDE_DIR)

APPLICATION := notiz
OBJECTS := MyApp.o DashboardFrame.o EditNoteFrame.o Note.o
TEST_OBJECTS := test.o

all: $(APPLICATION)

$(APPLICATION): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(CXX) -c -o $@ $(WX_CXXFLAGS) $(CXXFLAGS) $<

test: $(TEST_OBJECTS) Note.o
	$(CXX) -o $@ $^ $(WX_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS)

$(TEST_OBJECTS): %.o: %.cpp
	$(CXX) -c -o $@ $(WX_CXXFLAGS) $(CXXFLAGS) $<

.PHONY: all test clean

clean:
	find . -name '*~' -o -name '*.o' -o -name $(APPLICATION) -o -name test | xargs rm
