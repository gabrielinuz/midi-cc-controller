/**
 * @file MainWindow.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación de la clase MainWindow.
 * @version 0.4
 * @date 2025-06-13
 */
#include "MainWindow.hpp"
#include "ConfigParser.hpp"
#include "SliderControl.hpp" // Se sigue necesitando para crear instancias
#include <FL/fl_ask.H>
#include <FL/Fl.H>
#include <FL/Fl_File_Chooser.H> // Necesario para diálogos de archivo
#include <algorithm>
#include <sstream>
#include <fstream> // Necesario para std::ofstream

MainWindow::MainWindow(int width, int height, const char* title, std::shared_ptr<MidiService> midiService)
    : m_midiService(midiService)
{
    m_window = new Fl_Window(width, height, title);
    m_window->begin();

    int current_y = 10;
    
    // --- Barra de Estado ---
    m_statusBox = new Fl_Box(10, current_y, width - 20, 25, "Status: Initializing...");
    m_statusBox->box(FL_THIN_UP_BOX);
    m_statusBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    current_y += 35;

    // --- Selector de Puerto MIDI ---
    new Fl_Box(10, current_y, 80, 25, "MIDI Port:");
    m_portChoice = new Fl_Choice(100, current_y, width - 110, 25);
    m_portChoice->callback(onPortSelected_static, this);
    current_y += 30;

    // --- Selector de Canal MIDI ---
    new Fl_Box(10, current_y, 80, 25, "MIDI Channel:");
    m_channelChoice = new Fl_Choice(100, current_y, 100, 25);
    for (int i = 1; i <= 16; ++i) {
        m_channelChoice->add(std::to_string(i).c_str());
    }
    m_channelChoice->value(m_currentMidiChannel);
    m_channelChoice->callback(onChannelSelected_static, this);
    current_y += 40;

    // --- Botones de Gestión de Patches ---
    m_loadPatchButton = new Fl_Button(10, current_y, 120, 25, "Cargar Patch...");
    m_loadPatchButton->callback(onLoadPatch_static, this);

    m_savePatchButton = new Fl_Button(140, current_y, 120, 25, "Guardar Patch...");
    m_savePatchButton->callback(onSavePatch_static, this);

    m_sendAllButton = new Fl_Button(270, current_y, 120, 25, "Enviar Todo");
    m_sendAllButton->callback(onSendAll_static, this);
    current_y += 35;

    // --- Grupo de Scroll para Controles Dinámicos ---
    m_scrollGroup = new Fl_Scroll(0, current_y, width, height - current_y);
    m_scrollGroup->type(Fl_Scroll::VERTICAL);
    m_scrollGroup->end(); 
    
    m_window->end();

    // CORRECCIÓN DE REDIMENSIONAMIENTO:
    // Se establece el grupo de scroll como el componente "resizable" de la ventana.
    // Esto hace que el área de scroll se expanda y contraiga con la ventana.
    m_window->resizable(m_scrollGroup);

    populateMidiPorts();
}

MainWindow::~MainWindow()
{
    delete m_window;
}

void MainWindow::populateMidiPorts()
{
    // ... (sin cambios en este método)
    if (!m_midiService) return;
    if (!m_midiService->getInitializationError().empty())
    {
        updateStatus("RtMidi Init Error: " + m_midiService->getInitializationError());
        fl_alert("RtMidi Init Error: %s", m_midiService->getInitializationError().c_str());
        m_portChoice->deactivate();
        return;
    }
    m_portChoice->clear();
    unsigned int nPorts = m_midiService->getPortCount();
    if (nPorts == 0)
    {
        m_portChoice->add("No MIDI ports found");
        m_portChoice->deactivate();
        updateStatus("No MIDI output ports found.");
    }
    else
    {
        for (unsigned int i = 0; i < nPorts; ++i)
        {
            m_portChoice->add(m_midiService->getPortName(i).c_str());
        }
        m_portChoice->value(0);
        onPortSelected();
    }
}


void MainWindow::loadControlsFromFile(const std::string& configFile)
{
    std::vector<SliderConfig> configs;
    if (!ConfigParser::parse(configFile, configs))
    {
        updateStatus("Error: Could not load or parse config.csv");
        fl_alert("Error loading config.csv. Check console output.");
        return;
    }
    
    if (configs.empty())
    {
        updateStatus("Config file is empty or contains no valid controls.");
        return;
    }

    m_scrollGroup->begin();
    int current_y = m_scrollGroup->y(); // Coordenada Y relativa al inicio del scroll
    const int control_width = m_scrollGroup->w() - 20; // Ancho de control, dejando margen para la barra de scroll

    m_controls.clear(); // Limpiar controles existentes
    Fl::lock(); // Bloquear FLTK para evitar redibujados intermedios (mejora rendimiento)
    for (const auto& cfg : configs)
    {
        auto slider = std::make_unique<SliderControl>(cfg, m_midiService);
        slider->createWidgets(0, current_y, control_width, slider->getHeight(), &m_currentMidiChannel);
        m_controls.push_back(std::move(slider));
        current_y += m_controls.back()->getHeight();
    }
    Fl::unlock();
    m_scrollGroup->end(); 
    m_scrollGroup->redraw();
}

void MainWindow::show(int argc, char** argv)
{
    m_window->show(argc, argv);
}

void MainWindow::updateStatus(const std::string& message)
{
    if (m_statusBox) 
    {
        m_statusBox->copy_label(message.c_str());
        Fl::check();
    }
}

// --- Implementación de los Callbacks Estáticos ---
void MainWindow::onPortSelected_static(Fl_Widget* w, void* userdata)
{ static_cast<MainWindow*>(userdata)->onPortSelected(); }

void MainWindow::onChannelSelected_static(Fl_Widget* w, void* userdata)
{ static_cast<MainWindow*>(userdata)->onChannelSelected(); }

void MainWindow::onLoadPatch_static(Fl_Widget* w, void* userdata)
{ static_cast<MainWindow*>(userdata)->onLoadPatch(); }

void MainWindow::onSavePatch_static(Fl_Widget* w, void* userdata)
{ static_cast<MainWindow*>(userdata)->onSavePatch(); }

void MainWindow::onSendAll_static(Fl_Widget* w, void* userdata)
{ static_cast<MainWindow*>(userdata)->onSendAll(); }

// --- Lógica de la Instancia para los Callbacks ---
void MainWindow::onPortSelected()
{
    // ... (sin cambios en este método)
    if (!m_midiService) return;
    m_midiService->closePort();
    unsigned int port_idx = m_portChoice->value();
    if (m_midiService->openPort(port_idx))
    {
        updateStatus("Opened MIDI port: " + m_midiService->getPortName(port_idx));
    }
    else
    {
        updateStatus("Error: Failed to open MIDI port.");
    }
}

void MainWindow::onChannelSelected()
{
    // ... (sin cambios en este método)
    m_currentMidiChannel = static_cast<unsigned char>(m_channelChoice->value());
    std::stringstream ss;
    ss << "MIDI Channel set to " << (int)m_currentMidiChannel + 1;
    updateStatus(ss.str());
}

/**
 * @brief Abre un diálogo para que el usuario seleccione un archivo de patch y lo carga.
 */
void MainWindow::onLoadPatch() {
    Fl_File_Chooser chooser(".",                        // Directorio inicial
                            "Patch Files (*.csv)",      // Filtro de archivos
                            Fl_File_Chooser::SINGLE,    // Tipo de selección
                            "Cargar Patch");            // Título de la ventana
    chooser.show();
    while(chooser.shown()) { Fl::wait(); }

    if (chooser.value() == nullptr) {
        updateStatus("Carga de patch cancelada.");
        return;
    }

    std::string filename = chooser.value();
    std::map<int, int> patchData;
    if (!ConfigParser::parsePatch(filename, patchData)) {
        updateStatus("Error al leer el archivo de patch: " + filename);
        fl_alert("No se pudo abrir o leer el archivo de patch. Ver la consola.");
        return;
    }

    if (patchData.empty()) {
        updateStatus("El archivo de patch está vacío o no es válido.");
        return;
    }
    
    // Actualizar los controles con los valores del patch
    int controls_updated = 0;
    Fl::lock();
    for (auto& control : m_controls) {
        int cc = control->getCcNumber();
        auto it = patchData.find(cc);
        if (it != patchData.end()) {
            // it->first es la clave (CC#), it->second es el valor
            control->setCurrentValue(it->second);
            controls_updated++;
        }
    }
    Fl::unlock();
    m_window->redraw(); // Redibujar toda la ventana para mostrar los cambios.

    std::stringstream ss;
    ss << "Patch '" << filename << "' cargado. " << controls_updated << " controles actualizados.";
    updateStatus(ss.str());
}

/**
 * @brief Abre un diálogo para que el usuario guarde el estado actual de los controles en un archivo de patch.
 */
void MainWindow::onSavePatch() {
    Fl_File_Chooser chooser(".", "Patch Files (*.csv)", Fl_File_Chooser::CREATE, "Guardar Patch Como");
    chooser.show();
    while(chooser.shown()) { Fl::wait(); }

    if (chooser.value() == nullptr) {
        updateStatus("Guardado de patch cancelado.");
        return;
    }
    
    std::string filename = chooser.value();
    // Asegurarse de que la extensión sea .csv
    if (filename.rfind(".csv") == std::string::npos) {
        filename += ".csv";
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        updateStatus("Error: No se pudo crear el archivo " + filename);
        fl_alert("No se pudo crear el archivo para guardar el patch.");
        return;
    }
    
    // Escribir cabecera
    file << "CC#;Description;Range;Value\n";

    // Escribir datos de cada control
    for (const auto& control : m_controls) {
        file << control->getCcNumber() << ";"
             << control->getDescription() << ";"
             << control->getRange() << ";"
             << control->getCurrentValue() << "\n";
    }

    file.close();
    updateStatus("Patch guardado en " + filename);
}

/**
 * @brief Envía el valor actual de todos los controles al puerto MIDI.
 */
void MainWindow::onSendAll() {
    if (!m_midiService || !m_midiService->isPortOpen()) {
        updateStatus("Error: No hay un puerto MIDI abierto para enviar los datos.");
        fl_alert("Por favor, selecciona un puerto MIDI válido primero.");
        return;
    }
    
    for (const auto& control : m_controls) {
        m_midiService->sendCcMessage(
            m_currentMidiChannel,
            static_cast<unsigned char>(control->getCcNumber()),
            static_cast<unsigned char>(control->getCurrentValue())
        );
    }
    
    updateStatus("Se han enviado los valores de todos los controles.");
}