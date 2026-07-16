#include "sha256.h"

#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>

// Convierte un arreglo de bytes a su representación hexadecimal
// en minúsculas, usando dos caracteres por byte.
std::string bytesToHex(const unsigned char* datos, size_t longitud) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < longitud; ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(datos[i]);
    }
    return oss.str();
}

// Calcula el hash SHA-256 de un archivo leyéndolo en bloques de
// 4096 bytes, usando la API de bajo nivel de OpenSSL
// (SHA256_Init / SHA256_Update / SHA256_Final).
std::string calcularSHA256(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo, std::ios::binary);
    if (!archivo.is_open()) {
        // No se pudo abrir el archivo: se retorna cadena vacía.
        return std::string();
    }

    SHA256_CTX contexto;
    SHA256_Init(&contexto);

    const size_t TAMANIO_BLOQUE = 4096;
    std::vector<char> buffer(TAMANIO_BLOQUE);

    while (archivo.good()) {
        archivo.read(buffer.data(), static_cast<std::streamsize>(TAMANIO_BLOQUE));
        std::streamsize bytesLeidos = archivo.gcount();
        if (bytesLeidos > 0) {
            SHA256_Update(&contexto, buffer.data(), static_cast<size_t>(bytesLeidos));
        }
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &contexto);

    return bytesToHex(hash, SHA256_DIGEST_LENGTH);
}
