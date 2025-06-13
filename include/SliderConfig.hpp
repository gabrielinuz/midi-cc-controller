/**
 * @file SliderConfig.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Define la estructura de datos para almacenar la configuración de un slider.
 * @version 0.3
 * @date 2025-06-12
 * * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * * @link http://www.apache.org/licenses/LICENSE-2.0
 * */
#pragma once

#include <string>

/**
 * @brief Estructura de datos que contiene la configuración para un control deslizante (slider).
 * @details Esta estructura es utilizada para pasar los datos parseados del archivo
 * de configuración a las clases que necesitan crear y gestionar los controles.
 */
struct SliderConfig 
{
    int cc_number;              ///< El número de Control Change (CC) MIDI (0-127).
    std::string description;    ///< El texto descriptivo que se mostrará en la GUI junto al slider.
    int min_value;              ///< El valor mínimo que el slider puede enviar (usualmente 0).
    int max_value;              ///< El valor máximo que el slider puede enviar (usualmente 127).
};