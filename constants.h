#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

/**
 * @brief Tamaño máximo de datos a generar (en número de elementos)
 * @note 60 millones de números de 64 bits ≈ 457 MB de datos
 */
constexpr int64_t M = 60'000'000;
/**
 * @brief Tamaño en bytes de cada número a almacenar
 * @note Para números enteros de 64 bits (int64_t)
 */
constexpr size_t NUM_SIZE = 8;
/**
 * @brief Tamaño de bloque para operaciones de I/O (en bytes)
 * @note Típico tamaño de bloque de sistemas de archivos (4KB)
 */
constexpr size_t B = 4096;
/**
 * @brief Cantidad de números que caben en un bloque
 * @note Calculado como B / NUM_SIZE
 * @note Representa la unidad básica de transferencia
 */
constexpr size_t b = B / NUM_SIZE;

#endif