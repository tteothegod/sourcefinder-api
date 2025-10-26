CC = g++
CFLAGS = -Wall -std=c++17 \
         -I$(PYTHON_INCLUDE) \
         -I$(PYTORCH_INCLUDE) \
         -I$(PYTORCH_INCLUDE)/torch/csrc/api/include \
         `python3-config --includes`

LDFLAGS = -L$(PYTHON_LIB) \
          -L$(PYTORCH_LIB) \
          -ltorch -ltorch_cpu -lc10 -lpython3.13 -ldl -framework CoreFoundation

PYTHON_INCLUDE ?= /opt/homebrew/include
PYTORCH_INCLUDE ?= /opt/homebrew/opt/pytorch/include
PYTHON_LIB ?= /opt/homebrew/opt/python@3.13/Frameworks/Python.framework/Versions/3.13/lib
PYTORCH_LIB ?= /opt/homebrew/opt/pytorch/lib

export DYLD_LIBRARY_PATH=$(PYTORCH_LIB):$DYLD_LIBRARY_PATH

EXEC_DIR = src/executables
CLASS_DIR = src/classFiles

TARGETS = DataPrep sourcedinerScraper TrainModel # Executables

CLASS_SOURCES = $(wildcard $(CLASS_DIR)/*.cpp)
CLASS_OBJECTS = $(CLASS_SOURCES:.cpp=.o)

all:
	@echo "Specify an executable to build, e.g., 'make DataPrep'."
	@echo "Available targets: $(TARGETS)"

DataPrep: $(EXEC_DIR)/DataPrep.cpp $(CLASS_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

TrainModel: $(EXEC_DIR)/TrainModel.cpp $(CLASS_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

Scraper: $(EXEC_DIR)/sourcefinderScraper.cpp $(CLASS_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(CLASS_OBJECTS)
