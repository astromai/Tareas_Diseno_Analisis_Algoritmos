# Compilador y banderas
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O3
LDFLAGS := 

# Nombre del ejecutable
TARGET := arity

# Archivos fuente y objetos
SRC := arity.cpp mergesort.cpp iostats.cpp
OBJ := $(SRC:.cpp=.o)
HEADERS := arity.h mergesort.h iostats.h constants.h

# Directorios temporales a limpiar
TEMP_DIRS := $(wildcard temp_*/)

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

# Regla para limpieza
clean:
	# Archivos objeto y ejecutable
	rm -f $(OBJ) $(TARGET)
	
	# Directorios temporales
	rm -rf $(TEMP_DIRS)
	
	# Archivos generados
	rm -rf data bin
	
	# Archivos temporales del editor
	find . -type f \( -name '*~' -o -name '*.tmp' \) -delete

# Dependencias específicas
arity.o: arity.h mergesort.h iostats.h
mergesort.o: mergesort.h iostats.h
iostats.o: iostats.h