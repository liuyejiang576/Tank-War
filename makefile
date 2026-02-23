# Makefile for TankWar

CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -g -O2

TARGET = tankwar

SOURCES = main.cpp \
          common.cpp \
          tank.cpp \
          bullet.cpp \
          game_map.cpp \
          logger.cpp \
          command_parser.cpp \
          ui_manager.cpp \
          ai_player.cpp \
          game_engine.cpp

HEADERS = common.h \
          tank.h \
          bullet.h \
          game_map.h \
          logger.h \
          command_parser.h \
          ui_manager.h \
          ai_player.h \
          game_engine.h

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Windows-compatible clean (use: mingw32-make clean)
# On Windows CMD/PowerShell, run manually: del /Q *.o tankwar *.log
clean:
	-del /Q *.o 2>nul
	-del /Q tankwar.exe 2>nul
	-del /Q tankwar 2>nul
	-del /Q *.log 2>nul

distclean: clean
	-del /Q *~ *.bak 2>nul

test: $(TARGET)
	.\$(TARGET).exe --help

debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

release: CXXFLAGS += -DNDEBUG -O3
release: clean $(TARGET)

main.o: main.cpp game_engine.h command_parser.h common.h
common.o: common.cpp common.h
tank.o: tank.cpp tank.h common.h
bullet.o: bullet.cpp bullet.h tank.h common.h
game_map.o: game_map.cpp game_map.h tank.h common.h
logger.o: logger.cpp logger.h
command_parser.o: command_parser.cpp command_parser.h common.h
ui_manager.o: ui_manager.cpp ui_manager.h game_engine.h tank.h bullet.h game_map.h common.h
ai_player.o: ai_player.cpp ai_player.h game_engine.h tank.h bullet.h game_map.h common.h
game_engine.o: game_engine.cpp game_engine.h tank.h bullet.h game_map.h logger.h ui_manager.h ai_player.h common.h

.PHONY: all clean distclean test debug release help

help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove object files and executable"
	@echo "  distclean- Remove all generated files"
	@echo "  test     - Run basic test"
	@echo "  debug    - Build debug version"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help message"