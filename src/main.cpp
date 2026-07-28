#include "integrity.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sqlite3.h>
#include <ctime>
#include <fstream>

namespace {

// --- Funcionalidad estrella: historial SQLite ---
const char* DB_FILE = "integrity_history.db";

void initDatabase() {
    sqlite3* db;
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        std::cerr << "Warning: Could not open history database." << std::endl;
        return;
    }
    const char* sql = "CREATE TABLE IF NOT EXISTS scans ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "timestamp TEXT NOT NULL,"
                      "directory TEXT NOT NULL,"
                      "files_checked INTEGER NOT NULL,"
                      "modified INTEGER NOT NULL,"
                      "new_files INTEGER NOT NULL,"
                      "deleted INTEGER NOT NULL);";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    sqlite3_close(db);
}

void addScanToHistory(const std::string& directory, int checked, int modified, int newFiles, int deleted) {
    sqlite3* db;
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) return;

    time_t now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    const char* sql = "INSERT INTO scans (timestamp, directory, files_checked, modified, new_files, deleted) "
                      "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, timestamp, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, directory.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, checked);
        sqlite3_bind_int(stmt, 4, modified);
        sqlite3_bind_int(stmt, 5, newFiles);
        sqlite3_bind_int(stmt, 6, deleted);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void showHistory() {
    sqlite3* db;
    if (sqlite3_open(DB_FILE, &db) != SQLITE_OK) {
        std::cerr << "Could not open history database." << std::endl;
        return;
    }
    const char* sql = "SELECT timestamp, directory, files_checked, modified, new_files, deleted FROM scans ORDER BY id DESC LIMIT 20;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::cout << "Recent scans:\n";
        std::cout << "Timestamp\t\tDirectory\tChecked\tModified\tNew\tDeleted\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::cout << sqlite3_column_text(stmt, 0) << "\t"
                      << sqlite3_column_text(stmt, 1) << "\t"
                      << sqlite3_column_int(stmt, 2) << "\t"
                      << sqlite3_column_int(stmt, 3) << "\t"
                      << sqlite3_column_int(stmt, 4) << "\t"
                      << sqlite3_column_int(stmt, 5) << std::endl;
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void imprimirUso() {
    std::cout << "File Integrity Monitor - part of the Nexus Ecosystem\n"
              << "GitHub: https://github.com/Alonex-x/file-integrity-monitor\n\n"
              << "Upgrade to File Integrity Monitor PRO for:\n"
              << "  - Web dashboard and SMTP/Telegram alerts\n"
              << "  - Real-time monitoring (inotify)\n"
              << "  - Critical path alerts and ransomware detection\n"
              << "  - Multi-baseline management\n\n"
              << "  Get the PRO version at: [Gumroad link here]\n\n"
              << "Usage:\n"
              << "  file-integrity-monitor generate <directorio> [archivo_salida]\n"
              << "  file-integrity-monitor verify <directorio> [archivo_hashes] [--verbose]\n"
              << "  file-integrity-monitor history\n"
              << "  file-integrity-monitor --version\n";
}

void imprimirVersion() {
    std::cout << "File Integrity Monitor v1.3.0\n"
              << "PRO version available at: [Gumroad link here]\n";
}

bool directorioExiste(const std::string& ruta) {
    struct stat info;
    if (stat(ruta.c_str(), &info) != 0)
        return false;
    return (info.st_mode & S_IFDIR) != 0;
}

} // namespace

int main(int argc, char* argv[]) {
    initDatabase();

    if (argc < 2) { imprimirUso(); return 1; }
    std::string comando = argv[1];

    if (comando == "--help" || comando == "-h") { imprimirUso(); return 0; }
    if (comando == "--version") { imprimirVersion(); return 0; }
    if (comando == "history") { showHistory(); return 0; }

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
        bool ok = generarLineaBase(directorio, archivoSalida);
        if (ok) {
            // Contar archivos en el baseline (simplificado: contamos líneas no vacías)
            std::ifstream file(archivoSalida);
            int count = 0;
            std::string line;
            while (std::getline(file, line)) if (!line.empty()) count++;
            addScanToHistory(directorio, count, 0, 0, 0);
        }
        return ok ? 0 : 1;
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
        bool ok = verificarIntegridad(directorio, archivoHashes, verbose);
        if (ok) {
            // Nota: aquí podríamos obtener estadísticas reales de la verificación
            addScanToHistory(directorio, 0, 0, 0, 0);
        }
        return ok ? 0 : 1;
    }

    std::cerr << "Error: unknown command '" << comando << "'." << std::endl;
    imprimirUso();
    return 1;
}
