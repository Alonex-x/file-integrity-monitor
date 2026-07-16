```
# File Integrity Monitor

![C++](https://img.shields.io/badge/C++-17-blue?logo=c%2B%2B)

Herramienta de línea de comandos en C++ para monitorear la integridad de archivos mediante hashes SHA-256. Detecta modificaciones no autorizadas, archivos nuevos y eliminados comparando el estado actual con una línea base.

## Requisitos

- C++17
- OpenSSL (libssl-dev)
- make

## Compilación

```bash
make
```

## Uso

```bash
# Generar línea base de hashes
./file-integrity-monitor generate /ruta/a/monitorear

# Verificar integridad (sin verbose)
./file-integrity-monitor verify /ruta/a/monitorear

# Verificar integridad (con verbose)
./file-integrity-monitor verify /ruta/a/monitorear --verbose
```

## Ejemplo de salida

```
[FAIL] documentos/informe.pdf
  Esperado: a1b2c3d4...
  Actual:   e5f6g7h8...
[NEW] fotos/vacaciones.jpg
[MISSING] backup/old.sql
Resumen:
  Archivos OK:       12
  Modificados:       1
  Nuevos:            1
  Eliminados:        1
```

## Nota

Este proyecto es solo para fines educativos y de auditoría en sistemas propios o con autorización explícita.
```
