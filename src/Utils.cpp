/**
 * @file Utils.cpp
 * @author Gabriel Nicolás González Ferreira
 * @brief Implementación para funciones de utilidad generales.
 * @version 0.6
 * @date 2025-06-17
 */
#include "Utils.hpp"
#include <algorithm> // Para std::max

namespace Utils
{
    std::string getFileNameFromPath(const std::string& filepath)
    {
        // Buscar la última ocurrencia de '/' (para sistemas tipo Unix)
        size_t last_slash = filepath.find_last_of('/');
        // Buscar la última ocurrencia de '\' (para sistemas Windows)
        size_t last_backslash = filepath.find_last_of('\\');

        // Determinar cuál es el último separador válido
        size_t last_separator = std::string::npos; // No encontrado
        if (last_slash != std::string::npos)
        {
            last_separator = last_slash;
        }
        if (last_backslash != std::string::npos)
        {
            // Si hay ambos, tomamos el más reciente (el que está más a la derecha)
            if (last_separator == std::string::npos || last_backslash > last_separator) 
            {
                last_separator = last_backslash;
            }
        }

        if (last_separator == std::string::npos) 
        {
            // No se encontró ningún separador, la ruta es solo el nombre del archivo
            return filepath;
        }
        else
        {
            // Se encontró un separador, devuelve la subcadena después de él
            return filepath.substr(last_separator + 1);
        }
    }
} // namespace Utils