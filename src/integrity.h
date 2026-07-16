#ifndef INTEGRITY_H
#define INTEGRITY_H

#include <string>
#include <vector>

// Representa una entrada de la línea base: el hash SHA-256 de un
// archivo junto con su ruta relativa al directorio monitoreado.
struct EntradaHash {
    std::string hash;         // hash hexadecimal de 64 caracteres
    std::string rutaRelativa; // ruta relativa al directorio base
};

// Recorre recursivamente 'directorio', calcula el SHA-256 de cada
// archivo válido (ignorando ocultos y el propio archivo de salida)
// y guarda las entradas en 'archivoSalida' con el formato:
//   <hash> *<ruta_relativa>
// Retorna true si el proceso se completó correctamente.
bool generarLineaBase(const std::string& directorio, const std::string& archivoSalida);

// Compara el estado actual de 'directorio' contra la línea base
// almacenada en 'archivoHashes', reportando archivos modificados,
// eliminados y nuevos. Si 'verbose' es true, también reporta los
// archivos que están OK.
// Retorna true si no se detectaron cambios (todo OK).
bool verificarIntegridad(const std::string& directorio, const std::string& archivoHashes, bool verbose);

#endif // INTEGRITY_H
