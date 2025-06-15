# Compilador y flags
CXX = g++
CXXFLAGS = -O2 -std=c++17 -I.
BUILD_DIR = build

# Ejecutables
TARGETS = test_kruskal grilla

# Archivos fuente comunes
SRCS = main.cpp \
       estructuras/puntos.cpp \
       estructuras/aristas.cpp \
       estructuras/union_find.cpp \
       kruskal/kruskal.cpp

# Archivos fuente para grilla
GRILLA_SRCS = estructuras/grilla.cpp \
              estructuras/puntos.cpp \
              estructuras/aristas.cpp \
              estructuras/union_find.cpp \
              kruskal/kruskal.cpp

# Archivos objeto
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
GRILLA_OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(GRILLA_SRCS))

# Regla principal
.PHONY: all clean

all: $(TARGETS)

# Ejecutable test_kruskal
test_kruskal: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Ejecutable grilla con SDL2
grilla: $(GRILLA_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ `sdl2-config --cflags --libs`

# Regla genérica para compilar .cpp en .o dentro de build/
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -rf $(BUILD_DIR) $(TARGETS)
