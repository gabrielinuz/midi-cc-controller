/**
 * @file Utils.hpp
 * @author Gabriel Nicolás González Ferreira
 * @brief Archivo de cabecera para funciones de utilidad generales.
 * @version 0.6
 * @date 2025-06-17
 */
#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace Utils
{
    /**
     * @brief Extrae el nombre del archivo de una ruta completa.
     * Por ejemplo, "/home/user/documento.txt" -> "documento.txt"
     * "C:\\Users\\user\\file.doc" -> "file.doc"
     * @param filepath La ruta completa del archivo.
     * @return El nombre del archivo sin la ruta del directorio.
     */
    std::string getFileNameFromPath(const std::string& filepath);

    /**
     * @brief Extrae el directorio de una ruta de archivo completa.
     * @param filePath La ruta completa del archivo (ej: /home/user/presets/my_preset.csv).
     * @return El directorio que contiene el archivo (ej: /home/user/presets/).
     */
    std::string getDirectoryFromPath(const std::string& filePath);
} // namespace Utils

#endif // UTILS_HPP