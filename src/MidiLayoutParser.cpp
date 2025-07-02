/**
 * @file MidiLayoutParser.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación del parser para archivos de diseño (layout) de controladores MIDI.
 * @version 0.5
 * @date 2025-06-14
 */
#include "MidiLayoutParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // Para std::remove

namespace MidiLayoutParser
{
    bool parse(const std::string& filename, std::vector<SliderConfig>& configs)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open MIDI layout file: " << filename << std::endl;
            return false;
        }

        std::string line;
        // Leer la primera línea (cabecera) y descartarla
        if (!std::getline(file, line))
        {
            // Si el archivo está vacío (solo la cabecera o nada), no es un error de parseo, simplemente no hay configuraciones.
            return true;
        }

        // Limpiar el vector de configuraciones antes de cargar nuevas
        configs.clear();

        // Procesar cada línea del archivo.
        while (std::getline(file, line))
        {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); // Eliminar retornos de carro
            if (line.empty()) continue; // Saltar líneas vacías

            std::stringstream ss(line);
            std::string segment;
            SliderConfig current_config;

            try
            {
                // Leer Descripción (campo 1)
                std::getline(ss, segment, ';');
                current_config.description = segment;

                // Leer CC# (campo 2)
                std::getline(ss, segment, ';');
                current_config.cc_number = std::stoi(segment);

                // Leer Rango (campo 3) - Formato "min-max"
                std::getline(ss, segment, ';');
                size_t dash_pos = segment.find('-');
                if (dash_pos == std::string::npos)
                {
                    throw std::runtime_error("Invalid range format, expected 'min-max'");
                }
                current_config.min_value = std::stoi(segment.substr(0, dash_pos));
                current_config.max_value = std::stoi(segment.substr(dash_pos + 1));

                // Validar datos
                if (current_config.cc_number < 0 || current_config.cc_number > 127 ||
                    current_config.min_value < 0 || current_config.min_value > 127 ||
                    current_config.max_value < 0 || current_config.max_value > 127 ||
                    current_config.min_value > current_config.max_value)
                {
                    std::cerr << "Warning: Invalid data in layout line, skipping: " << line << std::endl;
                    continue; // Saltar línea inválida
                }

                configs.push_back(current_config);
            }
            catch(const std::exception& e)
            {
                std::cerr << "Error parsing MIDI layout line: '" << line << "'. Reason: " << e.what() << std::endl;
                // No retornar false aquí, solo se salta la línea problemática y se continúa.
            }
        }
        file.close();
        return true;
    }

} // namespace MidiLayoutParser