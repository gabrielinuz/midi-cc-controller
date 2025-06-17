/**
 * @file MidiPresetParser.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación del parser y guardador para archivos de presets MIDI.
 * @version 0.6
 * @date 2025-06-17
 */
#include "MidiPresetParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> 

namespace MidiPresetParser
{
    // @version 0.6: La firma de la función cambia para usar el nuevo struct PresetValue.
    bool load(const std::string& filename, std::map<int, PresetValue>& presetData)
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
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());// Eliminar retornos de carro
            if (line.empty() || line.rfind("CC#", 0) == 0) continue; // Saltar líneas vacías o la cabecera

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

                // NUEVO: Leer estado de activación (campo 3)
                bool active = true; // Por defecto es true para compatibilidad con presets antiguos
                if (std::getline(ss, segment, ';'))
                {
                    // Si hay un tercer campo, lo parseamos.
                    // Se aceptan "0" o "1". Cualquier otra cosa se trata como error o se ignora.
                    if (!segment.empty()) {
                        active = (std::stoi(segment) != 0);
                    }
                }

                if (cc_number >= 0 && cc_number <= 127 && value >= 0 && value <= 127)
                {
                    presetData[cc_number] = {value, active}; // Almacenar el struct
                }
                else
                {
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

        //  @version 0.6: Escribir nueva cabecera
        file << "CC#;Value;Active\n";

        // Escribir datos de cada control
        for (const auto& control : controls)
        {
            //  @version 0.6: Guardar el estado de activación
            file << control->getCcNumber() << ";"
                 << control->getCurrentValue() << ";"
                 << (control->isActive() ? "1" : "0") << "\n";
        }

        file.close();
        return true;
    }

} // namespace MidiPresetParser