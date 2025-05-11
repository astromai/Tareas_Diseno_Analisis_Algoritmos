# Variables
CC = g++
CFLAGS = -Wall -g
TARGET = aridad_io
SRC = aridad_io.cpp  # Especifica el archivo .cpp principal
OBJ = $(SRC:.cpp=.o) # Convierte el archivo .cpp en un archivo .o
TEMP_DIRS = $(wildcard temp*/)

# Regla principal (compila y enlaza)
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Regla para limpiar archivos y carpetas generadas
clean:
	# Elimina archivos de objetos
	rm -f $(OBJ)

	# Elimina el archivo principal ejecutable
	rm -f $(TARGET)

	# Elimina archivos generados temporalmente en cualquier carpeta 'temp'
	find . -type f -name '*~' -exec rm -f {} \;  # Elimina archivos temporales
	find . -type f -name '*.tmp' -exec rm -f {} \;  # Elimina archivos temporales con extensión .tmp

	# Elimina las carpetas temporales vacías o no vacías
	$(foreach dir, $(TEMP_DIRS), rm -rf $(dir))

	# Elimina la carpeta 'data' y su contenido
	rm -rf data

	# Elimina la carpeta 'bin' y su contenido
	rm -rf bin

# Regla para compilar el archivo .cpp en .o
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Regla de ejecución
run: $(TARGET)
	./$(TARGET)

# Regla de depuración (opcional)
debug:
	$(CC) $(SRC) -g -o $(TARGET)

.PHONY: all clean debug run
