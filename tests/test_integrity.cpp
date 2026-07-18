#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include "integrity.h"

namespace fs = std::filesystem;

namespace {
    void EscribirArchivo(const fs::path& ruta, const std::string& contenido) {
        fs::create_directories(ruta.parent_path());
        std::ofstream archivo(ruta, std::ios::binary);
        archivo << contenido;
    }
    std::string LeerArchivo(const fs::path& ruta) {
        std::ifstream archivo(ruta);
        std::ostringstream oss;
        oss << archivo.rdbuf();
        return oss.str();
    }
}

class IntegrityTest : public ::testing::Test {
protected:
    void SetUp() override {
        const std::string nombreTest = ::testing::UnitTest::GetInstance()->current_test_info()->name();
        directorioTemporal_ = fs::temp_directory_path() / ("fim_test_" + nombreTest);
        fs::remove_all(directorioTemporal_);
        fs::create_directories(directorioTemporal_);
        archivoHashes_ = (directorioTemporal_ / "hashes.sha256").string();
    }
    void TearDown() override { fs::remove_all(directorioTemporal_); }
    fs::path directorioTemporal_;
    std::string archivoHashes_;
};

TEST_F(IntegrityTest, GenerarLineaBaseCreaArchivoConEntradas) {
    EscribirArchivo(directorioTemporal_ / "a.txt", "contenido A");
    EscribirArchivo(directorioTemporal_ / "sub" / "b.txt", "contenido B");
    bool exito = generarLineaBase(directorioTemporal_.string(), archivoHashes_);
    ASSERT_TRUE(exito);
    ASSERT_TRUE(fs::exists(archivoHashes_));
    std::string contenido = LeerArchivo(archivoHashes_);
    EXPECT_NE(contenido.find("*a.txt"), std::string::npos);
    EXPECT_NE(contenido.find("*sub/b.txt"), std::string::npos);
}

TEST_F(IntegrityTest, VerificarSinCambiosRetornaTrue) {
    EscribirArchivo(directorioTemporal_ / "a.txt", "sin cambios");
    ASSERT_TRUE(generarLineaBase(directorioTemporal_.string(), archivoHashes_));
    bool todoOk = verificarIntegridad(directorioTemporal_.string(), archivoHashes_, false);
    EXPECT_TRUE(todoOk);
}

TEST_F(IntegrityTest, DetectaArchivoModificado) {
    fs::path rutaArchivo = directorioTemporal_ / "a.txt";
    EscribirArchivo(rutaArchivo, "contenido original");
    ASSERT_TRUE(generarLineaBase(directorioTemporal_.string(), archivoHashes_));
    EscribirArchivo(rutaArchivo, "contenido modificado");
    bool todoOk = verificarIntegridad(directorioTemporal_.string(), archivoHashes_, false);
    EXPECT_FALSE(todoOk);
}

TEST_F(IntegrityTest, DetectaArchivoNuevo) {
    EscribirArchivo(directorioTemporal_ / "a.txt", "contenido A");
    ASSERT_TRUE(generarLineaBase(directorioTemporal_.string(), archivoHashes_));
    EscribirArchivo(directorioTemporal_ / "nuevo.txt", "soy nuevo");
    bool todoOk = verificarIntegridad(directorioTemporal_.string(), archivoHashes_, false);
    EXPECT_FALSE(todoOk);
}

TEST_F(IntegrityTest, DetectaArchivoEliminado) {
    fs::path rutaArchivo = directorioTemporal_ / "a.txt";
    EscribirArchivo(rutaArchivo, "contenido A");
    ASSERT_TRUE(generarLineaBase(directorioTemporal_.string(), archivoHashes_));
    fs::remove(rutaArchivo);
    bool todoOk = verificarIntegridad(directorioTemporal_.string(), archivoHashes_, false);
    EXPECT_FALSE(todoOk);
}

TEST_F(IntegrityTest, IgnoraArchivosYCarpetasOcultas) {
    EscribirArchivo(directorioTemporal_ / "a.txt", "contenido A");
    EscribirArchivo(directorioTemporal_ / ".oculto.txt", "no debería aparecer");
    EscribirArchivo(directorioTemporal_ / ".carpeta_oculta" / "c.txt", "tampoco debería aparecer");
    ASSERT_TRUE(generarLineaBase(directorioTemporal_.string(), archivoHashes_));
    std::string contenido = LeerArchivo(archivoHashes_);
    EXPECT_EQ(contenido.find(".oculto.txt"), std::string::npos);
    EXPECT_EQ(contenido.find(".carpeta_oculta"), std::string::npos);
    bool todoOk = verificarIntegridad(directorioTemporal_.string(), archivoHashes_, false);
    EXPECT_TRUE(todoOk);
}

TEST_F(IntegrityTest, IgnoraElPropioArchivoDeHashes) {
    EscribirArchivo(directorioTemporal_ / "a.txt", "contenido A");
    ASSERT_TRUE(generarLineaBase(directorioTemporal_.string(), archivoHashes_));
    std::string contenido = LeerArchivo(archivoHashes_);
    EXPECT_EQ(contenido.find("hashes.sha256"), std::string::npos);
    bool todoOk = verificarIntegridad(directorioTemporal_.string(), archivoHashes_, false);
    EXPECT_TRUE(todoOk);
}

TEST_F(IntegrityTest, ArchivoDeHashesInexistenteRetornaFalse) {
    std::string rutaInexistente = (directorioTemporal_ / "no_existe.sha256").string();
    bool resultado = verificarIntegridad(directorioTemporal_.string(), rutaInexistente, false);
    EXPECT_FALSE(resultado);
}
