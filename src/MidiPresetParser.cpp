/**
 * @file MidiPresetParser.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación del parser y guardador para archivos de presets MIDI.
 * @version 0.5
 * @date 2025-06-14
 */
#include "MidiPresetParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> // Para std::remove

namespace MidiPresetParser
{
    bool load(const std::string& filename, std::map<int, int>& presetData)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open MIDI preset file: " << filename << std::endl;
            return false;
        }

        std::string line;
        // Leer la primera línea (cabecera) y descartarla
        if (!std::getline(file, line))
        {
            return true; // Archivo vacío (o solo cabecera) no es un error.
        }

        presetData.clear(); // Limpiar el mapa antes de cargar nuevos datos.

        // Procesar cada línea del archivo.
        while (std::getline(file, line))
        {
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end()); // Eliminar retornos de carro
            if (line.empty()) continue; // Saltar líneas vacías

            std::stringstream ss(line);
            std::string segment;

            try
            {
                // Leer CC# (campo 1)
                std::getline(ss, segment, ';');
                int cc_number = std::stoi(segment);

                // Leer Valor (campo 2)
                std::getline(ss, segment, ';');
                int value = std::stoi(segment);

                // Validar datos
                if (cc_number >= 0 && cc_number <= 127 && value >= 0 && value <= 127) {
                    presetData[cc_number] = value;
                } else {
                     std::cerr << "Warning: Invalid data in preset line, skipping: " << line << std::endl;
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << "Error parsing MIDI preset line: '" << line << "'. Reason: " << e.what() << std::endl;
                continue; // Saltar línea problemática
            }
        }
        file.close();
        return true;
    }

    bool save(const std::string& filename, const std::vector<std::unique_ptr<IMidiControl>>& controls)
    {
        std::ofstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not create/open MIDI preset file for writing: " << filename << std::endl;
            return false;
        }

        // Escribir cabecera simplificada
        file << "CC#;Value\n";

        // Escribir datos de cada control
        for (const auto& control : controls)
        {
            file << control->getCcNumber() << ";"
                 << control->getCurrentValue() << "\n";
        }

        file.close();
        return true;
    }

} // namespace MidiPresetParser