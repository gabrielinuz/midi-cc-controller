/**
 * @file MainWindow.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Gestiona la ventana principal de la aplicación y todos sus widgets.
 * @version 0.5
 * @date 2025-06-14
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
#include "SliderConfig.hpp" // Para recibir la configuración del layout

/**
 * @class MainWindow
 * @brief Orquesta la interfaz gráfica de usuario (GUI).
 * @details Esta clase es responsable de crear la ventana, los widgets estáticos
 * (selectores de puerto/canal, barra de estado), los botones de gestión de patches,
 * y de cargar dinámicamente los controles MIDI.
 * @version 0.5: Separación de la carga de layout y preset. Nuevo botón "Load Layout".
 * El guardado/carga de preset ahora solo maneja CC# y Value.
 */
class MainWindow
{
    public:
        /**
        * @brief Construye un nuevo objeto MainWindow.
        * @param width El ancho inicial de la ventana.
        * @param height La altura inicial de la ventana.
        * @param title El título de la ventana.
        * @param midiService Un puntero compartido al servicio MIDI.
        */
        MainWindow(int width, int height, const char* title, std::shared_ptr<MidiService> midiService);

        /**
         * @brief Destruye el objeto MainWindow.
         * @details Se encarga de liberar los widgets de FLTK y los controles MIDI dinámicos.
         */
        ~MainWindow();

        /**
         * @brief Carga las configuraciones de los controles desde un archivo CSV de layout.
         * @param filename La ruta del archivo CSV con el diseño de los controles.
         * @return true Si el layout fue cargado exitosamente.
         * @return false Si hubo un error al cargar el layout.
         */
        bool loadMidiLayoutFromFile(const std::string& filename);

        /**
         * @brief Muestra la ventana.
         * @param argc El contador de argumentos de la línea de comandos.
         * @param argv El array de argumentos de la línea de comandos.
         */
        void show(int argc, char** argv);

        /** @brief Actualiza el texto de la barra de estado. */
        void updateStatus(const std::string& message);

    private:
        // --- Callbacks estáticos de FLTK (trampolines) ---
        static void onPortSelected_static(Fl_Widget* w, void* userdata);
        static void onChannelSelected_static(Fl_Widget* w, void* userdata);
        static void onLoadLayout_static(Fl_Widget* w, void* userdata); // Nuevo callback
        static void onLoadPreset_static(Fl_Widget* w, void* userdata);
        static void onSavePreset_static(Fl_Widget* w, void* userdata);
        static void onResetAll_static(Fl_Widget* w, void* userdata); // <-- NUEVO: Callback estático para reset
        static void onSendAll_static(Fl_Widget* w, void* userdata);

        // --- Métodos de instancia para la lógica de los callbacks ---
        void onPortSelected();
        void onChannelSelected();
        void onLoadLayout(); // Nuevo método de instancia
        void onLoadPreset();
        void onSavePreset();
        void onResetAll(); // <-- NUEVO: Método de instancia para reset
        void onSendAll();

        /** @brief Llena el menú desplegable de puertos MIDI. */
        void populateMidiPorts();

        /**
         * @brief Elimina todos los controles MIDI dinámicos actuales de la GUI.
         * @details Esto se llama antes de cargar un nuevo layout para limpiar la interfaz.
         */
        void clearDynamicControls();

        /**
         * @brief Crea y añade un nuevo control deslizante a la interfaz.
         * @param config La configuración del slider.
         * @param y_position La posición Y donde se debe dibujar el slider.
         */
        void addSliderControl(const SliderConfig& config, int y_position);

        // --- Widgets de FLTK ---
        Fl_Window* m_window;
        Fl_Box* m_statusBox;
        Fl_Choice* m_portChoice;
        Fl_Choice* m_channelChoice;
        Fl_Scroll* m_scrollGroup;

        // Nuevos botones para la gestión de layout/presets
        Fl_Button* m_loadLayoutButton; // Nuevo botón
        Fl_Button* m_loadPresetButton;
        Fl_Button* m_savePresetButton;
        Fl_Button* m_resetAllButton; // <-- NUEVO: Botón de reset
        Fl_Button* m_sendAllButton;

        // --- Dependencias y Estado ---
        std::shared_ptr<MidiService> m_midiService;
        unsigned char m_currentMidiChannel = 0; // Canal MIDI seleccionado actualmente (0-15)

        /// @brief Vector de punteros únicos a las interfaces de control MIDI dinámicas.
        /// La ventana es dueña de estos controles.
        std::vector<std::unique_ptr<IMidiControl>> m_controls;

        /// @version 0.7: Variables atributos miembro para recordar las rutas ---
        std::string m_lastLayoutPath;
        std::string m_lastPresetPath;
};