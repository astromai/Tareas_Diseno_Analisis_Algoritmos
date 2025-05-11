# Compilador y banderas
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O0
LDFLAGS := -lstdc++fs  # Necesario para std::filesystem

# Nombre del ejecutable
TARGET := experiment

# Archivos fuente y objetos
SRC := mergesort.cpp iostats.cpp experiment.cpp quicksort.cpp
OBJ := $(SRC:.cpp=.o)
HEADERS := mergesort.h iostats.h constants.h quicksort.h experiment.h

# Directorios temporales a limpiar
TEMP_DIRS := $(wildcard temp_* temp_quick_*)

.PHONY: all clean run debug

all: $(TARGET)

# Regla principal de compilación
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Regla para archivos objeto
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regla para ejecutar
run: $(TARGET)
	./$(TARGET)

# Regla para depuración
debug: CXXFLAGS += -g
debug: clean $(TARGET)

# Regla para limpieza completa
clean:
	# Archivos objeto y ejecutable
	rm -f $(OBJ) $(TARGET)
	
	# Directorios temporales de ordenamiento
	rm -rf $(TEMP_DIRS)
	
	# Directorios de datos y resultados
	rm -rf data dataExp results bin
	
	# Archivos temporales del editor
	find . -type f \( -name '*~' -o -name '*.tmp' -o -name '*.bin' \) -delete

# Dependencias específicas
mergesort.o: mergesort.h iostats.h constants.h
quicksort.o: quicksort.h iostats.h constants.h
iostats.o: iostats.h constants.h
experiment.o: experiment.h mergesort.h quicksort.h iostats.h constants.h