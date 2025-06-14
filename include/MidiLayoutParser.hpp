/**
 * @file MidiLayoutParser.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Provee una función para parsear archivos de diseño (layout) de controladores MIDI en formato CSV.
 * @version 0.5
 * @date 2025-06-14
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */
#pragma once

#include "SliderConfig.hpp"
#include <vector>
#include <string>

/**
 * @namespace MidiLayoutParser
 * @brief Agrupa la funcionalidad de parseo de archivos de configuración de diseño (layout) de controles MIDI.
 * @details Usamos un namespace en lugar de una clase porque la funcionalidad
 * es sin estado (stateless). No necesitamos instanciar un objeto para parsear un archivo.
 * Este parser se encarga de leer la definición estructural de los sliders (CC#, descripción, rango).
 */
namespace MidiLayoutParser
{
    /**
     * @brief Parsea un archivo de configuración de diseño en formato CSV y lo carga en un vector de SliderConfig.
     * @param filename La ruta del archivo a parsear (ej: "synth_layout.csv").
     * @param[out] configs El vector donde se almacenarán las configuraciones de slider leídas.
     * @return true Si el archivo pudo ser abierto y parseado exitosamente.
     * @return false Si el archivo no pudo ser abierto o si hubo un error de formato.
     */
    bool parse(const std::string& filename, std::vector<SliderConfig>& configs);

} // namespace MidiLayoutParser