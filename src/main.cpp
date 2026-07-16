#include "integrity.h"
#include <iostream>
#include <string>

namespace {
void imprimirUso() {
    std::cout << "Uso:\n"
              << "  file-integrity-monitor generate <directorio>\n"
              << "  file-integrity-monitor verify <directorio> [--verbose]\n";
}
}

int main(int argc, char* argv[]) {
    if (argc < 2) { imprimirUso(); return 1; }
    std::string comando = argv[1];
    if (comando == "--help" || comando == "-h") { imprimirUso(); return 0; }

    if (comando == "generate") {
        if (argc != 3) {
            std::cerr << "Error: número de argumentos inválido para 'generate'." << std::endl;
            imprimirUso(); return 1;
        }
        std::string directorio = argv[2];
        std::string archivoSalida = directorio + "/hashes.sha256";
        return generarLineaBase(directorio, archivoSalida) ? 0 : 1;
    }

    if (comando == "verify") {
        if (argc != 3 && argc != 4) {
            std::cerr << "Error: número de argumentos inválido para 'verify'." << std::endl;
            imprimirUso(); return 1;
        }
        std::string directorio = argv[2];
        bool verbose = false;
        if (argc == 4) {
            std::string opcion = argv[3];
            if (opcion == "--verbose") verbose = true;
            else {
                std::cerr << "Error: opción desconocida '" << opcion << "'." << std::endl;
                imprimirUso(); return 1;
            }
        }
        std::string archivoHashes = directorio + "/hashes.sha256";
        return verificarIntegridad(directorio, archivoHashes, verbose) ? 0 : 1;
    }

    std::cerr << "Error: comando desconocido '" << comando << "'." << std::endl;
    imprimirUso();
    return 1;
}
