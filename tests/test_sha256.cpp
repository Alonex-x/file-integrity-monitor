#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include "sha256.h"

namespace fs = std::filesystem;

namespace {
    fs::path CrearArchivoTemporal(const std::string& contenido) {
        const std::string nombreTest = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        fs::path ruta = fs::temp_directory_path() / ("sha256_test_" + nombreTest + ".tmp");
        std::ofstream archivo(ruta, std::ios::binary);
        archivo << contenido;
        archivo.close();
        return ruta;
    }
}

TEST(SHA256Test, HashDeArchivoConContenidoConocido) {
    fs::path ruta = CrearArchivoTemporal("abc");
    std::string hashObtenido = calcularSHA256(ruta.string());
    EXPECT_EQ(hashObtenido, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    fs::remove(ruta);
}

TEST(SHA256Test, HashDeArchivoVacio) {
    fs::path ruta = CrearArchivoTemporal("");
    std::string hashObtenido = calcularSHA256(ruta.string());
    EXPECT_EQ(hashObtenido, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    fs::remove(ruta);
}

TEST(SHA256Test, ArchivoInexistenteRetornaCadenaVacia) {
    fs::path rutaInexistente = fs::temp_directory_path() / "este_archivo_no_deberia_existir_12345.txt";
    fs::remove(rutaInexistente);
    std::string hashObtenido = calcularSHA256(rutaInexistente.string());
    EXPECT_TRUE(hashObtenido.empty());
}

TEST(SHA256Test, BytesToHexConvierteCorrectamente) {
    const unsigned char datos[] = {0x00, 0xff, 0x1a, 0x09};
    std::string resultado = bytesToHex(datos, sizeof(datos));
    EXPECT_EQ(resultado, "00ff1a09");
}

TEST(SHA256Test, BytesToHexConLongitudCeroRetornaVacio) {
    const unsigned char datos[] = {0xAB};
    std::string resultado = bytesToHex(datos, 0);
    EXPECT_TRUE(resultado.empty());
}
