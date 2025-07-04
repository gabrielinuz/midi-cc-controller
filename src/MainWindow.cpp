/**
 * @file MainWindow.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación de la clase MainWindow.
 * @version 0.6
 * @date 2025-06-17
 */
#include "MainWindow.hpp"
#include "MidiLayoutParser.hpp"
#include "MidiPresetParser.hpp"
#include "SliderControl.hpp"    // Se sigue necesitando para crear instancias
#include "Utils.hpp" // Para la funciones de utilidad
#include <FL/fl_ask.H>
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H> // Necesario para diálogos de archivo
#include <FL/fl_draw.H> /// @version 0.6: Incluir para fl_font() y fl_measure()
#include <algorithm>
#include <sstream>
#include <fstream>
#include <map> // Para cargar presets

/// <-- @version 0.7: inicializar estas rutas a un valor por defecto, como el directorio actual "."
MainWindow::MainWindow(int width, int height, const char* title, std::shared_ptr<MidiService> midiService)
    : m_midiService(midiService), m_lastLayoutPath("."), m_lastPresetPath(".")
{
    m_window = new Fl_Window(width, height, title);
    m_window->begin();

    int current_y = 10;

    // --- Barra de Estado ---
    m_statusBox = new Fl_Box(10, current_y, width - 20, 25, "Status: Initializing...");
    m_statusBox->box(FL_THIN_UP_BOX);
    m_statusBox->align(FL_ALIGN_LEFT | FL_ALIGN_WRAP | FL_ALIGN_INSIDE);
    current_y += 35;

    // --- Selector de Puerto MIDI ---
    new Fl_Box(10, current_y, 80, 25, "MIDI Port:");
    m_portChoice = new Fl_Choice(100, current_y, 280, 25);
    m_portChoice->callback(onPortSelected_static, this);
    populateMidiPorts();
    current_y += 35;

    // --- Selector de Canal MIDI ---
    new Fl_Box(10, current_y, 80, 25, "MIDI Channel:");
    m_channelChoice = new Fl_Choice(100, current_y, 100, 25);
    for (int i = 1; i <= 16; ++i) 
    {
        m_channelChoice->add(std::to_string(i).c_str());
    }
    m_channelChoice->value(0); // Por defecto Canal 1 (índice 0)
    m_currentMidiChannel = 0;  // Inicializar el estado interno
    m_channelChoice->callback(onChannelSelected_static, this);
    current_y += 35;

    // --- Botones de Gestión de Layout y Presets ---
    int button_x = 10;
    int button_width = 107;
    int button_height = 25;
    int button_spacing = 10;

    m_loadLayoutButton = new Fl_Button(button_x, current_y, button_width, button_height, "Load Layout");
    m_loadLayoutButton->callback(onLoadLayout_static, this);
    button_x += button_width + button_spacing;

    m_loadPresetButton = new Fl_Button(button_x, current_y, button_width, button_height, "Load Preset");
    m_loadPresetButton->callback(onLoadPreset_static, this);
    button_x += button_width + button_spacing;

    m_savePresetButton = new Fl_Button(button_x, current_y, button_width, button_height, "Save Preset");
    m_savePresetButton->callback(onSavePreset_static, this);
    button_x += button_width + button_spacing;

    // --- Botón de Reset All ---
    // Lo ubicamos al lado de los botones de patch
    m_resetAllButton = new Fl_Button(button_x, current_y, button_width, button_height, "Reset All");
    m_resetAllButton->callback(onResetAll_static, this);
    button_x += button_width + button_spacing; // esto es necesario para el espacio entre botones

    m_sendAllButton = new Fl_Button(button_x, current_y, button_width, button_height, "Send All");
    m_sendAllButton->callback(onSendAll_static, this);
    current_y += 35;

    // --- Grupo de Scroll para Controles Dinámicos ---
    // El scroll group contendrá todos los sliders MIDI.
    // Su posición y tamaño inicial se ajustará, pero permitirá scroll si hay muchos controles.
    m_scrollGroup = new Fl_Scroll(10, current_y, width - 20, height - current_y - 10);
    m_scrollGroup->type(Fl_Scroll::VERTICAL);
    m_scrollGroup->begin(); // Los widgets que se creen a continuación serán hijos de m_scrollGroup

    // Aquí no se crean sliders inicialmente. Se harán con loadMidiLayoutFromFile.

    m_scrollGroup->end(); // Fin del grupo de scroll
    m_window->end(); // Fin de la ventana principal

    // Hacer la ventana redimensionable y ajustar su mínimo tamaño.
    m_window->resizable(m_window);
    m_window->size_range(width, 250); // Mínimo 250 de altura para asegurar que los controles estáticos sean visibles
}

MainWindow::~MainWindow()
{
    // Los widgets hijos de Fl_Window se destruyen automáticamente cuando la ventana es destruida.
    // Solo necesitamos limpiar los unique_ptr de m_controls.
    clearDynamicControls();
}

/**
 * @brief Elimina todos los controles MIDI dinámicos actuales de la GUI.
 * @details Esto se llama antes de cargar un nuevo layout para limpiar la interfaz.
 */
void MainWindow::clearDynamicControls()
{
    if (m_scrollGroup)
    {
        m_scrollGroup->clear(); // Elimina todos los widgets hijos de Fl_Scroll
    }
    m_controls.clear(); // Limpia el vector de unique_ptr
}

/**
 * @brief Crea y añade un nuevo control deslizante a la interfaz.
 * @param config La configuración del slider.
 * @param y_position La posición Y donde se debe dibujar el slider dentro del scroll group.
 */
void MainWindow::addSliderControl(const SliderConfig& config, int y_position)
{
    // El SliderControl necesita un puntero al canal MIDI actual para sus callbacks.
    // Se le pasa la dirección de m_currentMidiChannel.
    auto sliderControl = std::make_unique<SliderControl>(config, m_midiService);
    sliderControl->createWidgets(10, y_position, m_scrollGroup->w() - 20, 45, &m_currentMidiChannel);
    // Establecer el callback del slider individual
    sliderControl->getFlSlider()->callback(SliderControl::sliderCallback_static, sliderControl.get());
    m_controls.push_back(std::move(sliderControl));
}


/**
 * @brief Carga las configuraciones de los controles desde un archivo CSV de layout.
 * @param filename La ruta del archivo CSV con el diseño de los controles.
 * @return true Si el layout fue cargado exitosamente.
 * @return false Si hubo un error al cargar el layout.
 */
bool MainWindow::loadMidiLayoutFromFile(const std::string& filename)
{
    std::string display_name = Utils::getFileNameFromPath(filename); /// @version 0.6 - solo el nombre
    std::vector<SliderConfig> configs;
    if (!MidiLayoutParser::parse(filename, configs))
    {
        updateStatus("Error loading MIDI layout from " + display_name);
        fl_alert(("Error al cargar el layout MIDI desde:\n" + display_name).c_str());
        return false;
    }

    // Limpiar los controles existentes antes de crear nuevos
    clearDynamicControls();

    if (configs.empty())
    {
        updateStatus("Warning: No slider configurations found in " + display_name);
        fl_alert(("Advertencia: No se encontraron configuraciones de sliders en:\n" + display_name + "\nEl controlador estará vacío.").c_str());
        return true; // No es un error crítico si el archivo está vacío pero se abrió correctamente.
    }

    // Volver a establecer el grupo de scroll como el grupo actual para añadir widgets.
    m_scrollGroup->begin();
    int current_y_in_scroll = 0; // Posición Y dentro del grupo de scroll
    int slider_height = 45; // Altura de cada grupo de slider (label + slider)
    int slider_spacing = 5; // Espacio entre sliders

    for (const auto& config : configs)
    {
        addSliderControl(config, current_y_in_scroll);
        current_y_in_scroll += (slider_height + slider_spacing);
    }
    m_scrollGroup->end();

    // Ajustar el tamaño del scroll group para que contenga todos los sliders
    // y permitir el scroll si es necesario.
    m_scrollGroup->init_sizes(); // Esto recalcula el tamaño interno del scroll group.

    // Ajustar la altura de la ventana si los nuevos controles exceden la altura inicial.
    // Esto asegura que la ventana se adapte al contenido.
    int minimum_height_for_controls = m_scrollGroup->y() + current_y_in_scroll + 10;
    m_window->size(m_window->w(), std::max(m_window->h(), minimum_height_for_controls));
    m_window->redraw(); // Forzar el redibujado de la ventana y sus hijos.

    updateStatus("MIDI layout loaded from " + display_name + ". " + std::to_string(configs.size()) + " sliders created.");
    return true;
}

void MainWindow::show(int argc, char** argv)
{
    m_window->show(argc, argv);
    // Asegurarse de que el puerto MIDI esté inicializado y seleccionado si hay alguno disponible.
    onPortSelected();
}

/** @version 0.6 Actualización de este método para que el texto se ajuste deinámicamente*/
void MainWindow::updateStatus(const std::string& message)
{
    if (m_statusBox)
    {
        // 1. Establecer la fuente y el tamaño globales para la medición,
        //    usando los mismos que tiene nuestra barra de estado.
        fl_font(m_statusBox->labelfont(), m_statusBox->labelsize());

        // 2. Medir el ancho y alto que ocupará el nuevo texto.
        int text_width = 0;
        int text_height = 0;
        fl_measure(message.c_str(), text_width, text_height);

        // 3. Añadir un padding horizontal (ej: 20 píxeles en total) para que se vea mejor.
        int box_width = text_width + 20;

        // 4. Redimensionar el widget Fl_Box. Mantenemos la misma altura.
        m_statusBox->size(box_width, m_statusBox->h());
        
        // 5. Actualizamos el texto.
        m_statusBox->copy_label(message.c_str());

        // @version 0.7 6. Redibujar el PADRE para limpiar artefactos. m_statusBox->redraw() no iba.
        m_window->redraw(); // <-- SOLUCIÓN
    }
}

// --- Callbacks Estáticos (Trampolines) ---
void MainWindow::onPortSelected_static(Fl_Widget* w, void* userdata)
{
    static_cast<MainWindow*>(userdata)->onPortSelected();
}

void MainWindow::onChannelSelected_static(Fl_Widget* w, void* userdata)
{
    static_cast<MainWindow*>(userdata)->onChannelSelected();
}

void MainWindow::onLoadLayout_static(Fl_Widget* w, void* userdata)
{
    static_cast<MainWindow*>(userdata)->onLoadLayout();
}

void MainWindow::onLoadPreset_static(Fl_Widget* w, void* userdata)
{
    static_cast<MainWindow*>(userdata)->onLoadPreset();
}

void MainWindow::onSavePreset_static(Fl_Widget* w, void* userdata)
{
    static_cast<MainWindow*>(userdata)->onSavePreset();
}

/**
 * @brief Callback estático para el botón "Reset All".
 */
void MainWindow::onResetAll_static(Fl_Widget* w, void* userdata)
{ 
    static_cast<MainWindow*>(userdata)->onResetAll();
}

void MainWindow::onSendAll_static(Fl_Widget* w, void* userdata)
{
    static_cast<MainWindow*>(userdata)->onSendAll();
}

// --- Lógica de Callbacks de Instancia ---
void MainWindow::onPortSelected()
{
    if (!m_midiService)
    {
        updateStatus("Error: MIDI service not available.");
        return;
    }

    //Leer NOTES.md #1 para entender por qué es importante cerrar primero los puertos si están abiertos.
    if (m_midiService->isPortOpen()) 
    {
        m_midiService->closePort();
    }

    int port_index = m_portChoice->value();
    if (port_index < 0 || port_index >= (int)m_midiService->getPortCount())
    {
        updateStatus("No MIDI port selected or available.");
        m_midiService->closePort();
        return;
    }

    std::string port_name = m_midiService->getPortName(port_index);
    if (m_midiService->openPort(port_index))
    {
        updateStatus("MIDI port " + port_name + " opened successfully.");
    }
    else
    {
        updateStatus("Failed to open MIDI port: " + port_name + ". " + m_midiService->getInitializationError());
        fl_alert(("No se pudo abrir el puerto MIDI:\n" + port_name + "\n" + m_midiService->getInitializationError()).c_str());
    }
}

void MainWindow::onChannelSelected()
{
    m_currentMidiChannel = static_cast<unsigned char>(m_channelChoice->value());
    updateStatus("MIDI Channel set to " + std::to_string(m_currentMidiChannel + 1));
}

/**
 * @brief Muestra un diálogo para seleccionar un archivo de layout MIDI y lo carga.
 */
void MainWindow::onLoadLayout()
{
    /// @version 0.7 Usar m_lastLayoutPath como valor inicial
    const char* filename = fl_file_chooser("Load MIDI Controller Layout", "*.csv", m_lastLayoutPath.c_str());
    if (filename)
    {
        /// @version 0.7 Actualizar la ruta para la próxima vez
        m_lastLayoutPath = Utils::getDirectoryFromPath(filename);
        loadMidiLayoutFromFile(filename);
    }
}

/**
 * @brief Muestra un diálogo para seleccionar un archivo de preset MIDI y carga sus valores.
 */
void MainWindow::onLoadPreset()
{
    if (m_controls.empty())
    {
        updateStatus("Error: No MIDI controls loaded. Please load a layout first.");
        fl_alert("No hay controles MIDI cargados. Por favor, carga un archivo de diseño (layout) primero.");
        return;
    }

    const char* filename = fl_file_chooser("Load MIDI Preset", "*.csv", m_lastPresetPath.c_str());
    if (filename)
    {
        /// @version 0.7 Actualizar la ruta para la próxima vez
        m_lastPresetPath = Utils::getDirectoryFromPath(filename);

        /// @version 0.6 - solo el nombre, tiene que ir adentro o da error cuando sea nulo IMPORTANTE.
        std::string display_name = Utils::getFileNameFromPath(filename); 

        /// @version 0.6: Usar el nuevo mapa con el struct PresetValue.
        std::map<int, PresetValue> presetData;
        if (MidiPresetParser::load(filename, presetData))
        {
            int updated_count = 0;
            for (const auto& control : m_controls)
            {
                int cc_num = control->getCcNumber();
                if (presetData.count(cc_num))
                {
                    const auto& data = presetData.at(cc_num);
                    /// @version 0.6: Establecer tanto el valor como el estado de activación.
                    control->setCurrentValue(data.value);
                    control->setActive(data.active);
                    updated_count++;
                }
            }
            
            updateStatus("Preset loaded from " + std::string(display_name) + ". " + std::to_string(updated_count) + " controls updated.");
        }
        else
        {
            updateStatus("Error loading preset from " + std::string(display_name));
            fl_alert(("Error al cargar el preset MIDI desde:\n" + std::string(display_name)).c_str());
        }
    }
}

/**
 * @brief Muestra un diálogo para guardar el estado actual de los controles como un preset MIDI.
 */
void MainWindow::onSavePreset()
{
    if (m_controls.empty())
    {
        updateStatus("No sliders to save.");
        fl_alert("No hay sliders cargados para guardar un preset.");
        return;
    }

    const char* filename_char = fl_file_chooser("Save MIDI Preset As", "*.csv", "preset.csv", 1);
    
    if (filename_char)
    {
        std::string filename = filename_char;
        std::string display_name = Utils::getFileNameFromPath(filename); /// @version 0.6 - solo el nombre
        // Asegurarse de que la extensión sea .csv
        if (filename.rfind(".csv") == std::string::npos)
        {
            filename += ".csv";
        }

        if (MidiPresetParser::save(filename, m_controls))
        {
            updateStatus("Preset saved to " + display_name);
        }
        else
        {
            updateStatus("Error saving preset to " + display_name);
            fl_alert(("No se pudo crear el archivo para guardar el preset:\n" + display_name).c_str());
        }
    }
}

/**
 * @brief Implementa la lógica para resetear todos los controles a sus valores mínimos (0).
 */
void MainWindow::onResetAll() 
{
    if (m_controls.empty()) 
    {
        updateStatus("No hay controles para resetear.");
        return;
    }

    int reset_count = 0;
    for (const auto& control : m_controls) 
    {
        /// @version 0.6: Solo resetear y enviar si el control está activo.
        if (control->isActive())
        {
            control->setCurrentValue(0); // <-- Establece el valor a 0 (o control->m_config.min_value)

            /*Después de establecer el valor, 
            necesitamos simular el envío MIDI 
            o directamente enviar el mensaje 
            si setCurrentValue no lo hace.*/            
            m_midiService->sendCcMessage(m_currentMidiChannel, static_cast<unsigned char>(control->getCcNumber()), 0);
            reset_count++;
        }
    }
    updateStatus(std::to_string(reset_count) + " active controls have been reset to 0.");
}

/**
 * @brief Envía el valor actual de todos los controles al puerto MIDI.
 */
void MainWindow::onSendAll()
{
    if (!m_midiService || !m_midiService->isPortOpen())
    {
        updateStatus("Error: No hay un puerto MIDI abierto para enviar los datos.");
        fl_alert("Por favor, selecciona un puerto MIDI válido primero.");
        return;
    }

    int sent_count = 0;
    for (const auto& control : m_controls)
    {
        /// @version 0.6: Solo enviar el mensaje si el control está activo.
        if (control->isActive())
        {
            m_midiService->sendCcMessage(
                m_currentMidiChannel,
                static_cast<unsigned char>(control->getCcNumber()),
                static_cast<unsigned char>(control->getCurrentValue())
            );
            sent_count++;
        }
    }
    updateStatus("Sent " + std::to_string(sent_count) + " active MIDI CC messages on Channel " + std::to_string(m_currentMidiChannel + 1) + ".");
}

/** 
 * @brief Llena el menú desplegable de puertos MIDI. 
 */
void MainWindow::populateMidiPorts()
{
    if (!m_midiService) return;
    
    m_portChoice->clear();
    unsigned int num_ports = m_midiService->getPortCount();
    if (num_ports == 0)
    {
        m_portChoice->add("No MIDI ports found");
        m_portChoice->deactivate();
        updateStatus("No MIDI output ports found.");
        return;
    }

    for (unsigned int i = 0; i < num_ports; ++i)
    {
        m_portChoice->add(m_midiService->getPortName(i).c_str());
    }
    m_portChoice->value(0); // Seleccionar el primer puerto por defecto
    m_portChoice->activate();
    updateStatus("MIDI ports found. Select a port.");
}