/**
 * @file Aplication.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación del Application.hpp inicializando y conectando los componentes principales
 * @version 0.4
 * @date 2025-06-13
 */
#include "Application.hpp"
#include <FL/Fl.H>

Application::Application()
{
    // 1. Crear el servicio MIDI. Se usa shared_ptr porque será compartido con los controles.
    m_midiService = std::make_shared<MidiService>();

    // 2. Crear la ventana principal, inyectando el servicio MIDI.
    //    Se usa unique_ptr porque la aplicación es la única dueña de la ventana.
    m_mainWindow = std::make_unique<MainWindow>(600, 400, "MIDI CC Editor", m_midiService);
}

int Application::run(int argc, char** argv)
{
    // Procesar argumentos de línea de comandos específicos de FLTK.
    Fl::args(argc, argv);
    Fl::visual(FL_RGB); // Mejorar la apariencia visual.

    // Por defecto, cargar un layout inicial si existe, si no, se cargará por el usuario.
    // Se recomienda que el usuario cargue el layout explícitamente.
    // Si quieres un layout por defecto, descomenta la línea siguiente y asegura que 'config.csv' exista.
    // m_mainWindow->loadMidiLayoutFromFile("config.csv");
    
    // Mostrar la ventana.
    m_mainWindow->show(argc, argv);

    // Iniciar el bucle de eventos de FLTK. Esta función bloqueará la ejecución
    // hasta que todas las ventanas se cierren.
    return Fl::run();
}