/**
 * @file ConfigParser.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación del parser de archivos de configuración y patches.
 * @version 0.4
 * @date 2025-06-13
 */
#include "ConfigParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace ConfigParser 
{

    // La función parse original no se modifica.
    bool parse(const std::string& filename, std::vector<SliderConfig>& configs)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open config file: " << filename << std::endl;
            return false;
        }

        std::string line;
        if (!std::getline(file, line))
        {
            return true;
        }

        while (std::getline(file, line))
        {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string segment;
            SliderConfig current_config;

            try
            {
                std::getline(ss, segment, ';');
                current_config.cc_number = std::stoi(segment);

                std::getline(ss, segment, ';');
                current_config.description = segment;

                std::getline(ss, segment, ';');
                size_t dash_pos = segment.find('-');
                if (dash_pos != std::string::npos)
                {
                    current_config.min_value = std::stoi(segment.substr(0, dash_pos));
                    current_config.max_value = std::stoi(segment.substr(dash_pos + 1));
                }
                else
                {
                    throw std::runtime_error("Invalid range format, expected 'min-max'");
                }

                if (current_config.cc_number < 0 || current_config.cc_number > 127 ||
                    current_config.min_value < 0 || current_config.max_value > 127 ||
                    current_config.min_value > current_config.max_value)
                {
                    std::cerr << "Warning: Invalid data range in line, skipping: " << line << std::endl;
                    continue;
                }
                configs.push_back(current_config);

            }
            catch (const std::exception& e)
            {
                std::cerr << "Error parsing line: '" << line << "'. Reason: " << e.what() << std::endl;
                continue;
            }
        }
        file.close();
        return true;
    }

    // --- Nueva función para parsear archivos de patch ---
    bool parsePatch(const std::string& filename, std::map<int, int>& patchData)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open patch file: " << filename << std::endl;
            return false;
        }

        patchData.clear(); // Limpiar datos anteriores
        std::string line;
        // Omitir la línea de cabecera del CSV, si existe.
        if (!std::getline(file, line))
        {
            return true; // Archivo vacío no es un error.
        }

        // Procesar cada línea del archivo.
        while (std::getline(file, line))
        {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string segment;
            
            try
            {
                // Leer CC# (campo 1)
                std::getline(ss, segment, ';');
                int cc_number = std::stoi(segment);

                // Omitir Descripción (campo 2)
                std::getline(ss, segment, ';');

                // Omitir Rango (campo 3)
                std::getline(ss, segment, ';');

                // Leer Valor (campo 4)
                std::getline(ss, segment, ';');
                int value = std::stoi(segment);
                
                // Validar datos
                if (cc_number >= 0 && cc_number <= 127 && value >= 0 && value <= 127) {
                    patchData[cc_number] = value;
                } else {
                     std::cerr << "Warning: Invalid data in patch line, skipping: " << line << std::endl;
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "Error parsing patch line: '" << line << "'. Reason: " << e.what() << std::endl;
                continue;
            }
        }
        file.close();
        return true;
    }

} // namespace ConfigParser