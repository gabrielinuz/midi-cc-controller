/**
 * @file ConfigParser.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Provee una función para parsear el archivo de configuración CSV.
 * @version 0.4
 * @date 2025-06-13
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */
#pragma once

#include "SliderConfig.hpp"
#include <vector>
#include <string>
#include <map> // Necesario para std::map

/**
 * @namespace ConfigParser
 * @brief Agrupa la funcionalidad de parseo de archivos.
 * @details Usamos un namespace en lugar de una clase porque la funcionalidad
 * es sin estado (stateless). No necesitamos instanciar un objeto para parsear un archivo.
 */
namespace ConfigParser 
{
    /**
     * @brief Parsea un archivo de configuración en formato CSV y lo carga en un vector de SliderConfig.
     * @param filename La ruta del archivo a parsear (ej: "config.csv").
     * @param[out] configs El vector donde se almacenarán las configuraciones leídas.
     * @return true Si el archivo pudo ser abierto.
     * @return false Si el archivo no pudo ser abierto.
     */
    bool parse(const std::string& filename, std::vector<SliderConfig>& configs);

    /**
     * @brief Parsea un archivo de patch en formato CSV y carga los datos en un mapa.
     * @details El formato esperado es: CC#;Description;Range;Value.
     * Solo se utilizan los campos CC# y Value.
     * El mapa resultante asocia un número de CC a su valor.
     * @param filename La ruta del archivo de patch a parsear.
     * @param[out] patchData El mapa donde se almacenarán los pares (CC#, Valor).
     * @return true Si el archivo pudo ser abierto.
     * @return false Si el archivo no pudo ser abierto.
     */
    bool parsePatch(const std::string& filename, std::map<int, int>& patchData);
}