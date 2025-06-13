/**
 * @file Application.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Clase principal que orquesta el ciclo de vida de la aplicación.
 * @version 0.3
 * @date 2025-06-12
 * * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * * @link http://www.apache.org/licenses/LICENSE-2.0
 * */
#pragma once

#include "MainWindow.hpp"
#include "MidiService.hpp"
#include <memory>

/**
 * @class Application
 * @brief Punto de entrada y clase orquestadora de la aplicación.
 * @details Su responsabilidad es crear y conectar los componentes principales
 * (como el MidiService y la MainWindow) y iniciar el bucle de eventos.
 * Esto mantiene la función `main()` extremadamente simple.
 */
class Application 
{
    public:
        /**
        * @brief Construye un nuevo objeto Application.
        * @details Inicializa todos los componentes principales de la aplicación.
        */
        Application();

        /**
        * @brief Ejecuta el ciclo de vida principal de la aplicación.
        * * @param argc El contador de argumentos de la línea de comandos.
        * @param argv El array de argumentos de la línea de comandos.
        * @return int El código de salida de la aplicación.
        */
        int run(int argc, char** argv);

    private:
        /// @brief Puntero compartido al servicio MIDI, que será inyectado en otras clases.
        std::shared_ptr<MidiService> m_midiService;

        /// @brief Puntero único a la ventana principal. La aplicación es dueña de la ventana.
        std::unique_ptr<MainWindow> m_mainWindow;
};