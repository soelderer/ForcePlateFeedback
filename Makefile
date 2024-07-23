.SUFFIXES:
.PRECIOUS: %.o
.PHONY: all compile checkstyle test clean

QT_DIR = /usr
MOC = /usr/lib/qt6/moc
CXX = clang++
CXXFLAGS = -I$(QT_DIR)/include/qt6 -Wall -Wextra -Wdeprecated -fsanitize=address -g -std=c++17
MAIN_BINARY = $(basename $(wildcard *Main.cpp))
TEST_BINARY = $(basename $(wildcard *Test.cpp))
LIBS = -lncurses -lQt6Core -lQt6Gui -lQt6Widgets
TESTLIBS = -lgtest -lgtest_main -lpthread
OBJECTS = $(addsuffix .o, $(basename $(filter-out %Main.cpp %Test.cpp, $(wildcard *.cpp))))
MOC_OBJECTS = moc_ForcePlateFeedback.o moc_DataModel.o

all: compile checkstyle test

compile: $(MAIN_BINARY) $(TEST_BINARY)

checkstyle:
	clang-format-14 --dry-run -Werror *.h *.cpp

test: $(TEST_BINARY)
	./$<

%.o: %.cpp *.h
	$(CXX) $(CXXFLAGS) -c $<

# rule for generating moc files
moc_%.cpp: %.h
	$(MOC) $< -o $@
	clang-format-14 -i $@

moc_%.o: moc_%h.cpp *.h
	$(CXX) $(CXXFLAGS) -c $<

%Main: %Main.o $(OBJECTS) $(MOC_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%Test: %Test.o $(OBJECTS) $(MOC_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) $(TESTLIBS)

clean:
	rm -f *Main
	rm -f *Test
	rm -f *.o
	rm -f moc_*.cpp

format:
	clang-format-14 -i *.cpp *.h
