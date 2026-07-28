#include "integrity.h"
#include <iostream>
#include <string>
#include <sys/stat.h>

namespace {

void imprimirUso() {
    std::cout << "File Integrity Monitor - part of the Nexus Ecosystem\n"
              << "GitHub: https://github.com/Alonex-x/file-integrity-monitor\n\n"
              << "Upgrade to File Integrity Monitor PRO for:\n"
              << "  - Web dashboard and SMTP/Telegram alerts\n"
              << "  - Real-time monitoring (inotify)\n"
              << "  - Critical path alerts and ransomware detection\n"
              << "  - Multi-baseline management and history\n\n"
              << "  Get the PRO version at: [Gumroad link here]\n\n"
              << "Usage:\n"
              << "  file-integrity-monitor generate <directorio> [archivo_salida]\n"
              << "  file-integrity-monitor verify <directorio> [archivo_hashes] [--verbose]\n"
              << "  file-integrity-monitor --version\n";
}

void imprimirVersion() {
    std::cout << "File Integrity Monitor v1.2.0\n"
              << "PRO version available at: [Gumroad link here]\n";
}

bool directorioExiste(const std::string& ruta) {
    struct stat info;
    if (stat(ruta.c_str(), &info) != 0)
        return false;
    return (info.st_mode & S_IFDIR) != 0;
}

}

int main(int argc, char* argv[]) {
    if (argc < 2) { imprimirUso(); return 1; }
    std::string comando = argv[1];

    if (comando == "--help" || comando == "-h") { imprimirUso(); return 0; }
    if (comando == "--version") { imprimirVersion(); return 0; }

    if (comando == "generate") {
        if (argc < 3 || argc > 4) {
            std::cerr << "Error: invalid number of arguments for 'generate'." << std::endl;
            imprimirUso(); return 1;
        }
        std::string directorio = argv[2];
        if (!directorioExiste(directorio)) {
            std::cerr << "Error: directory '" << directorio << "' does not exist." << std::endl;
            return 1;
        }
        std::string archivoSalida = (argc == 4) ? argv[3] : directorio + "/hashes.sha256";
        return generarLineaBase(directorio, archivoSalida) ? 0 : 1;
    }

    if (comando == "verify") {
        if (argc < 3 || argc > 5) {
            std::cerr << "Error: invalid number of arguments for 'verify'." << std::endl;
            imprimirUso(); return 1;
        }
        std::string directorio = argv[2];
        if (!directorioExiste(directorio)) {
            std::cerr << "Error: directory '" << directorio << "' does not exist." << std::endl;
            return 1;
        }
        bool verbose = false;
        std::string archivoHashes;
        if (argc >= 4) {
            std::string arg3 = argv[3];
            if (arg3 == "--verbose") {
                verbose = true;
            } else {
                archivoHashes = arg3;
                if (argc == 5) {
                    std::string arg4 = argv[4];
                    if (arg4 == "--verbose") verbose = true;
                    else {
                        std::cerr << "Error: unknown option '" << arg4 << "'." << std::endl;
                        imprimirUso(); return 1;
                    }
                }
            }
        }
        if (archivoHashes.empty()) {
            archivoHashes = directorio + "/hashes.sha256";
        }
        return verificarIntegridad(directorio, archivoHashes, verbose) ? 0 : 1;
    }

    std::cerr << "Error: unknown command '" << comando << "'." << std::endl;
    imprimirUso();
    return 1;
}
