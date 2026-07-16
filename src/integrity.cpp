#include "integrity.h"
#include "sha256.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

namespace fs = std::filesystem;

namespace {

bool tieneComponenteOculto(const fs::path& rutaRelativa) {
    for (const auto& parte : rutaRelativa) {
        const std::string texto = parte.string();
        if (!texto.empty() && texto[0] == '.') {
            return true;
        }
    }
    return false;
}

std::vector<std::string> listarArchivosValidos(const fs::path& directorio, const fs::path& rutaExcluida) {
    std::vector<std::string> resultado;
    fs::path excluidaAbsoluta = fs::weakly_canonical(rutaExcluida);

    for (auto it = fs::recursive_directory_iterator(
             directorio, fs::directory_options::skip_permission_denied);
         it != fs::recursive_directory_iterator(); ++it) {

        const fs::directory_entry& entrada = *it;

        if (entrada.is_directory()) {
            const std::string nombre = entrada.path().filename().string();
            if (!nombre.empty() && nombre[0] == '.') {
                it.disable_recursion_pending();
            }
            continue;
        }

        if (!entrada.is_regular_file()) continue;

        fs::path rutaRelativa = fs::relative(entrada.path(), directorio);
        if (tieneComponenteOculto(rutaRelativa)) continue;
        if (fs::weakly_canonical(entrada.path()) == excluidaAbsoluta) continue;

        resultado.push_back(rutaRelativa.generic_string());
    }
    std::sort(resultado.begin(), resultado.end());
    return resultado;
}
} // namespace

bool generarLineaBase(const std::string& directorio, const std::string& archivoSalida) {
    fs::path rutaDirectorio(directorio);
    if (!fs::exists(rutaDirectorio) || !fs::is_directory(rutaDirectorio)) {
        std::cerr << "Error: el directorio '" << directorio << "' no existe." << std::endl;
        return false;
    }

    std::ofstream salida(archivoSalida, std::ios::trunc);
    if (!salida.is_open()) {
        std::cerr << "Error: no se pudo crear el archivo de salida '" << archivoSalida << "'." << std::endl;
        return false;
    }

    std::vector<std::string> archivos = listarArchivosValidos(rutaDirectorio, fs::path(archivoSalida));
    int contador = 0;
    for (const std::string& rutaRelativa : archivos) {
        fs::path rutaCompleta = rutaDirectorio / fs::path(rutaRelativa);
        std::string hash = calcularSHA256(rutaCompleta.string());
        if (hash.empty()) {
            std::cerr << "Advertencia: no se pudo leer el archivo '" << rutaRelativa << "'." << std::endl;
            continue;
        }
        salida << hash << " *" << rutaRelativa << "\n";
        std::cout << "[GENERATE] " << rutaRelativa << " -> " << hash << std::endl;
        ++contador;
    }
    salida.close();
    std::cout << "Línea base generada: " << archivoSalida
              << " (" << contador << " archivos procesados)" << std::endl;
    return true;
}

bool verificarIntegridad(const std::string& directorio, const std::string& archivoHashes, bool verbose) {
    fs::path rutaDirectorio(directorio);
    if (!fs::exists(archivoHashes)) {
        std::cerr << "Error: el archivo de hashes '" << archivoHashes << "' no existe." << std::endl;
        return false;
    }

    std::ifstream entrada(archivoHashes);
    if (!entrada.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo de hashes '" << archivoHashes << "'." << std::endl;
        return false;
    }

    std::vector<EntradaHash> entradas;
    std::string linea;
    int numeroLinea = 0;
    while (std::getline(entrada, linea)) {
        ++numeroLinea;
        if (linea.empty()) continue;
        const size_t LONGITUD_HASH = 64;
        if (linea.size() <= LONGITUD_HASH + 2) {
            std::cerr << "Advertencia: línea " << numeroLinea << " con formato inválido, se ignora." << std::endl;
            continue;
        }
        std::string hash = linea.substr(0, LONGITUD_HASH);
        bool hashValido = (hash.size() == LONGITUD_HASH) &&
                           std::all_of(hash.begin(), hash.end(), [](unsigned char c) { return std::isxdigit(c); });
        if (!hashValido || linea[LONGITUD_HASH] != ' ' || linea[LONGITUD_HASH + 1] != '*') {
            std::cerr << "Advertencia: línea " << numeroLinea << " con formato inválido, se ignora." << std::endl;
            continue;
        }
        std::string rutaRelativa = linea.substr(LONGITUD_HASH + 2);
        if (rutaRelativa.empty()) {
            std::cerr << "Advertencia: línea " << numeroLinea << " con formato inválido, se ignora." << std::endl;
            continue;
        }
        entradas.push_back({hash, rutaRelativa});
    }
    entrada.close();

    int contadorOk = 0, contadorModificados = 0, contadorEliminados = 0;
    std::map<std::string, bool> rutasRegistradas;
    for (const EntradaHash& e : entradas) {
        rutasRegistradas[e.rutaRelativa] = true;
        fs::path rutaCompleta = rutaDirectorio / fs::path(e.rutaRelativa);
        if (!fs::exists(rutaCompleta) || !fs::is_regular_file(rutaCompleta)) {
            std::cout << "[MISSING] " << e.rutaRelativa << std::endl;
            ++contadorEliminados;
            continue;
        }
        std::string hashActual = calcularSHA256(rutaCompleta.string());
        if (hashActual == e.hash) {
            if (verbose) std::cout << "[OK] " << e.rutaRelativa << std::endl;
            ++contadorOk;
        } else {
            std::cout << "[FAIL] " << e.rutaRelativa << std::endl;
            std::cout << "  Esperado: " << e.hash << std::endl;
            std::cout << "  Actual:   " << (hashActual.empty() ? "(no se pudo leer)" : hashActual) << std::endl;
            ++contadorModificados;
        }
    }

    int contadorNuevos = 0;
    if (fs::exists(rutaDirectorio) && fs::is_directory(rutaDirectorio)) {
        std::vector<std::string> archivosActuales = listarArchivosValidos(rutaDirectorio, fs::path(archivoHashes));
        for (const std::string& rutaRelativa : archivosActuales) {
            if (rutasRegistradas.find(rutaRelativa) == rutasRegistradas.end()) {
                std::cout << "[NEW] " << rutaRelativa << std::endl;
                ++contadorNuevos;
            }
        }
    }

    std::cout << "Resumen:" << std::endl;
    std::cout << "  Archivos OK:       " << contadorOk << std::endl;
    std::cout << "  Modificados:       " << contadorModificados << std::endl;
    std::cout << "  Nuevos:            " << contadorNuevos << std::endl;
    std::cout << "  Eliminados:        " << contadorEliminados << std::endl;

    return (contadorModificados == 0 && contadorNuevos == 0 && contadorEliminados == 0);
}
