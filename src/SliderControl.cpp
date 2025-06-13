/**
 * @file SliderControl.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación de la clase SliderControl. 
 * \\Creación de widgets y el manejo de eventos para los sliders MIDI.
 * @version 0.5
 * @date 2025-06-13
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */
#include "SliderControl.hpp"
#include <string>
#include <sstream> // Para std::stringstream

SliderControl::SliderControl(const SliderConfig& config, std::shared_ptr<MidiService> midiService)
    : m_config(config),
      m_midiService(midiService),
      m_currentMidiChannel(nullptr),
      m_group(nullptr),
      m_label(nullptr),
      m_slider(nullptr),
      m_valueOutput(nullptr) // Inicializar el puntero del Fl_Value_Output
    {}

void SliderControl::createWidgets(int x, int y, int w, int h, unsigned char* currentMidiChannel) 
{
    m_currentMidiChannel = currentMidiChannel;
    
    m_group = new Fl_Group(x, y, w, h);
    m_group->begin();

    m_label = new Fl_Box(x + 10, y, 100, 25, m_config.description.c_str());
    m_label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
    
    // CORRECCIÓN DEL TOOLTIP:
    // 1. El texto del tooltip se almacena en una variable miembro (m_tooltipText)
    //    para garantizar que el puntero de c_str() sea válido durante toda la vida del widget.
    // 2. El tooltip se asigna al grupo (m_group) en lugar de solo a la etiqueta.
    //    Esto hace que el tooltip aparezca al pasar el ratón sobre cualquier parte del control (etiqueta o slider).
    m_tooltipText = "CC# " + std::to_string(m_config.cc_number);
    m_group->tooltip(m_tooltipText.c_str());

    // El slider
    // Ajustamos la posición y el ancho para dejar espacio al Fl_Value_Output
    // ¡Línea corregida aquí! Se añadió 'nullptr' como sexto argumento para la etiqueta.
    m_slider = new Fl_Slider(FL_HORIZONTAL, x + 110, y, w - 170, 25, nullptr); // Reducimos el ancho del slider
    m_slider->bounds(m_config.min_value, m_config.max_value);
    m_slider->value(m_config.min_value); // Set initial value to min
    m_slider->step(1); // Para asegurar pasos enteros si los valores son enteros
    m_slider->callback(sliderCallback_static, this);
    m_slider->tooltip(m_tooltipText.c_str()); // El slider también puede tener el tooltip

    // ¡NUEVO! Widget Fl_Value_Output para mostrar el valor
    // Lo posicionamos a la derecha del slider
    m_valueOutput = new Fl_Value_Output(x + w - 55, y, 45, 25); // Posición y tamaño adecuados
    m_valueOutput->value(m_slider->value()); // Establecer el valor inicial
    m_valueOutput->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE); // Alinear el texto al centro
    m_valueOutput->labelsize(12); // Tamaño de fuente del valor

    m_group->end();
    m_group->resizable(m_slider); // Hacer el slider y su grupo redimensionables
}

Fl_Widget* SliderControl::getWidgetGroup()
{
    return m_group;
}

int SliderControl::getHeight() const
{
    // Definir una altura constante para este tipo de control.
    // Esto facilita el layout dinámico en la ventana principal.
    return 45; 
}

// --- Implementación de los nuevos métodos de la interfaz ---
int SliderControl::getCcNumber() const 
{
    return m_config.cc_number;
}

std::string SliderControl::getDescription() const 
{
    return m_config.description;
}

std::string SliderControl::getRange() const 
{
    // Usar stringstream para formatear el rango de manera eficiente.
    std::stringstream ss;
    ss << m_config.min_value << "-" << m_config.max_value;
    return ss.str();
}

int SliderControl::getCurrentValue() const 
{
    if (m_slider) 
    {
        return static_cast<int>(m_slider->value());
    }
    return 0; // Valor por defecto si el widget no está creado
}

void SliderControl::setCurrentValue(int value) 
{
    if (m_slider) 
    {
        // Asegurarse de que el valor esté dentro de los límites del slider.
        if (value < m_slider->minimum()) value = m_slider->minimum();
        if (value > m_slider->maximum()) value = m_slider->maximum();
        m_slider->value(value);
        m_slider->redraw(); // Forzar redibujado para que el cambio sea visible.
        if (m_valueOutput) 
        { // Actualizar también el Fl_Value_Output
            m_valueOutput->value(value);
            m_valueOutput->redraw();
        }
    }
}

// --- Lógica de Callbacks ---

void SliderControl::sliderCallback_static(Fl_Widget* w, void* userdata)
{
    static_cast<SliderControl*>(userdata)->sliderCallback();
}

void SliderControl::sliderCallback()
{
    if (!m_midiService || !m_currentMidiChannel)
    {
        return;
    }

    unsigned char cc_num = static_cast<unsigned char>(m_config.cc_number);
    unsigned char value = static_cast<unsigned char>(m_slider->value());
    unsigned char channel = *m_currentMidiChannel; // Usar el canal actual de MainWindow

    m_midiService->sendCcMessage(channel, cc_num, value);

    if (m_valueOutput) 
    { 
        // Actualizar el Fl_Value_Output en el callback:
        m_valueOutput->value(value);
        m_valueOutput->redraw();
    }
}