# Caso de estudio: File Integrity Monitor

**Problema.** Cuando un servidor o una carpeta con archivos sensibles es comprometida, uno de los primeros indicios suele ser que algún archivo fue modificado, reemplazado o eliminado sin autorización. Detectar esto a tiempo puede marcar la diferencia entre una alerta temprana y un incidente de seguridad que pasa desapercibido durante semanas.

**Solución.** File Integrity Monitor es una herramienta de línea de comandos escrita en C++ que calcula el hash SHA-256 de cada archivo dentro de un directorio y lo guarda como una "línea base" de referencia. En cualquier momento posterior, el usuario puede volver a ejecutar la herramienta en modo de verificación: esta recalcula los hashes actuales y los compara contra la línea base, reportando de forma clara qué archivos fueron modificados, cuáles son nuevos y cuáles desaparecieron.

**Resultado.** El proyecto ofrece una forma ligera y sin dependencias externas complejas (solo OpenSSL) de vigilar la integridad de un conjunto de archivos, ideal para carpetas de configuración, directorios web o cualquier ubicación donde un cambio inesperado deba llamar la atención de inmediato. Su suite de tests automatizados con Google Test, integrada en un pipeline de CI/CD, garantiza que la lógica de detección siga siendo confiable a medida que el proyecto evoluciona.
