/**
 * @file MainWindow.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Gestiona la ventana principal de la aplicación y todos sus widgets.
 * @version 0.4
 * @date 2025-06-13
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */

#pragma once

#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Button.H> // Necesario para Fl_Button
#include <memory>
#include <vector>
#include <string>

#include "MidiService.hpp"
#include "IMidiControl.hpp"

/**
 * @class MainWindow
 * @brief Orquesta la interfaz gráfica de usuario (GUI).
 * @details Esta clase es responsable de crear la ventana, los widgets estáticos
 * (selectores de puerto/canal, barra de estado), los botones de gestión de patches,
 * y de cargar dinámicamente los controles MIDI.
 * @version 0.4: Añadida funcionalidad de carga/guardado de patches y botón "Enviar Todo".
 * Mejorado el redimensionamiento de la ventana.
 */
class MainWindow 
{
    public:
        /**
        * @brief Construye un nuevo objeto MainWindow.
        * @param width El ancho inicial de la ventana.
        * @param height La altura inicial de la ventana.
        * @param title El título de la ventana.
        * @param midiService Un puntero compartido al MidiService.
        */
        MainWindow(int width, int height, const char* title, std::shared_ptr<MidiService> midiService);

        /** @brief Destruye el objeto MainWindow. */
        ~MainWindow();
        
        // Se deshabilita la copia para seguir las buenas prácticas con ventanas de GUI.
        MainWindow(const MainWindow&) = delete;
        MainWindow& operator=(const MainWindow&) = delete;

        /**
        * @brief Carga los controles (sliders) desde un archivo de configuración.
        * @param configFile La ruta del archivo de configuración.
        */
        void loadControlsFromFile(const std::string& configFile);

        /** @brief Muestra la ventana. */
        void show(int argc, char** argv);

        /** @brief Actualiza el texto de la barra de estado. */
        void updateStatus(const std::string& message);

    private:
        // --- Callbacks estáticos de FLTK (trampolines) ---
        static void onPortSelected_static(Fl_Widget* w, void* userdata);
        static void onChannelSelected_static(Fl_Widget* w, void* userdata);
        static void onLoadPatch_static(Fl_Widget* w, void* userdata);
        static void onSavePatch_static(Fl_Widget* w, void* userdata);
        static void onSendAll_static(Fl_Widget* w, void* userdata);

        // --- Métodos de instancia para la lógica de los callbacks ---
        void onPortSelected();
        void onChannelSelected();
        void onLoadPatch();
        void onSavePatch();
        void onSendAll();

        /** @brief Llena el menú desplegable de puertos MIDI. */
        void populateMidiPorts();

        // --- Widgets de FLTK ---
        Fl_Window* m_window;
        Fl_Box* m_statusBox;
        Fl_Choice* m_portChoice;
        Fl_Choice* m_channelChoice;
        Fl_Scroll* m_scrollGroup; 
        
        // Nuevos widgets para patches
        Fl_Button* m_loadPatchButton;
        Fl_Button* m_savePatchButton;
        Fl_Button* m_sendAllButton;

        // --- Dependencias y Estado ---
        std::shared_ptr<MidiService> m_midiService;
        unsigned char m_currentMidiChannel = 0;
        std::vector<std::unique_ptr<IMidiControl>> m_controls;
};