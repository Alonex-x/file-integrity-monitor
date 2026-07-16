#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <cstddef>

// Calcula el hash SHA-256 del archivo indicado y lo retorna como
// una cadena hexadecimal de 64 caracteres en minúsculas.
// Si el archivo no se puede abrir, retorna una cadena vacía.
std::string calcularSHA256(const std::string& rutaArchivo);

// Convierte un arreglo de bytes a su representación hexadecimal
// en minúsculas.
std::string bytesToHex(const unsigned char* datos, size_t longitud);

#endif // SHA256_H
