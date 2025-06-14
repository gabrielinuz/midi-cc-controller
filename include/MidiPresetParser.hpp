/**
 * @file MidiPresetParser.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Provee funciones para parsear y guardar archivos de presets MIDI en formato CSV.
 * @version 0.5
 * @date 2025-06-14
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */
#pragma once

#include "IMidiControl.hpp" // Necesario para acceder a getCurrentValue, getCcNumber, etc.
#include <string>
#include <map>
#include <vector>
#include <memory> // Para std::unique_ptr en save

/**
 * @namespace MidiPresetParser
 * @brief Agrupa la funcionalidad de parseo y guardado de archivos de presets MIDI.
 * @details Usamos un namespace en lugar de una clase porque la funcionalidad
 * es sin estado (stateless). No necesitamos instanciar un objeto para interactuar con archivos.
 * Este parser se encarga de leer y escribir los valores de los CCs que forman un preset.
 */
namespace MidiPresetParser
{
    /**
     * @brief Parsea un archivo de preset en formato CSV y carga los datos en un mapa.
     * @details El formato esperado es: CC#;Value.
     * El mapa resultante asocia un número de CC a su valor.
     * @param filename La ruta del archivo de preset a parsear.
     * @param[out] presetData El mapa donde se almacenarán los pares (CC#, Valor).
     * @return true Si el archivo pudo ser abierto y parseado exitosamente.
     * @return false Si el archivo no pudo ser abierto o si hubo un error de formato.
     */
    bool load(const std::string& filename, std::map<int, int>& presetData);

    /**
     * @brief Guarda el estado actual de un conjunto de controles MIDI en un archivo CSV.
     * @details El formato de guardado es: CC#;Value.
     * Solo los valores actuales de los controles se guardan.
     * @param filename La ruta del archivo donde se guardará el preset.
     * @param controls Un vector de punteros a IMidiControl, desde donde se obtendrán los datos.
     * @return true Si el preset fue guardado exitosamente.
     * @return false Si no se pudo crear o escribir en el archivo.
     */
    bool save(const std::string& filename, const std::vector<std::unique_ptr<IMidiControl>>& controls);

} // namespace MidiPresetParser