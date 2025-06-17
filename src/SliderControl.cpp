/**
 * @file SliderControl.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación de la clase SliderControl. 
 * \\Creación de widgets y el manejo de eventos para los sliders MIDI.
 * @version 0.6
 * @date 2025-06-17
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
      m_isActive(true), /** @version 0.6: Por defecto, un control está activo.*/
      m_group(nullptr),
      m_checkButton(nullptr),/** @version 0.6: Inicializar el puntero del checkbox*/
      m_label(nullptr),
      m_slider(nullptr),
      m_valueOutput(nullptr) // Inicializar el puntero del Fl_Value_Output
    {}

void SliderControl::createWidgets(int x, int y, int w, int h, unsigned char* currentMidiChannel) 
{
    m_currentMidiChannel = currentMidiChannel;
    
    m_group = new Fl_Group(x, y, w, h);
    m_group->begin();

    /// @version 0.6: Checkbox a la izquierda del todo.
    m_checkButton = new Fl_Check_Button(x + 5, y + 5, 20, 20);
    m_checkButton->value(m_isActive ? 1 : 0); /** @version 0.6:  Establecer su estado inicial*/
    m_checkButton->callback(onCheckboxClicked_static, this);    

    /// @version 0.6: Ajustar la posición y el ancho de los otros widgets para hacer espacio.
    m_label = new Fl_Box(x + 30, y, 100, 25, m_config.description.c_str());
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
    /// @version 0.6: Ajustar la posición del slider y su ancho
    m_slider = new Fl_Slider(FL_HORIZONTAL, x + 135, y, w - 195, 25, nullptr);
    m_slider->bounds(m_config.min_value, m_config.max_value);
    m_slider->value(m_config.min_value); // Set initial value to min
    m_slider->step(1); // Para asegurar pasos enteros si los valores son enteros
    m_slider->callback(sliderCallback_static, this);
    m_slider->tooltip(m_tooltipText.c_str()); // El slider también puede tener el tooltip

    /// @version 0.5: Widget Fl_Value_Output para mostrar el valor
    // Lo posicionamos a la derecha del slider
    ///@version 0.6: Ajustar la posición del value output
    m_valueOutput = new Fl_Value_Output(x + w - 55, y, 45, 25); // Posición y tamaño adecuados
    m_valueOutput->value(m_slider->value()); // Establecer el valor inicial
    m_valueOutput->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE); // Alinear el texto al centro
    m_valueOutput->labelsize(12); // Tamaño de fuente del valor

    m_group->end();
    m_group->resizable(m_slider); // Hacer el slider y su grupo redimensionables

    ///@version 0.6: Aplicar el estado de activación inicial a los widgets visuales
    setActive(m_isActive);
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

/// --- @version 0.6: Implementación de los métodos de activación ---

void SliderControl::setActive(bool active)
{
    m_isActive = active;
    if (m_checkButton)
    {
        m_checkButton->value(m_isActive ? 1 : 0);
    }
    if (m_slider) 
    {
        if (m_isActive) 
        {
            m_slider->activate();
            m_valueOutput->activate();
            m_label->activate();
        } 
        else 
        {
            m_slider->deactivate();
            m_valueOutput->deactivate();
            m_label->deactivate();
        }
        m_slider->redraw();
        m_valueOutput->redraw();
        m_label->redraw();
    }
}

bool SliderControl::isActive() const
{
    return m_isActive;
}


// --- Lógica de Callbacks ---

/// --- @version 0.6:
void SliderControl::onCheckboxClicked_static(Fl_Widget* w, void* userdata)
{
    static_cast<SliderControl*>(userdata)->onCheckboxClicked();
}

/// --- @version 0.6:
void SliderControl::onCheckboxClicked()
{
    if (m_checkButton)
    {
        // El nuevo estado es el valor del checkbox.
        // Lo pasamos a nuestro método setActive para actualizar todo.
        setActive(m_checkButton->value() != 0);
    }
}

void SliderControl::sliderCallback_static(Fl_Widget* w, void* userdata)
{
    static_cast<SliderControl*>(userdata)->sliderCallback();
}

void SliderControl::sliderCallback()
{
    /// --- @version 0.6: Solo enviar MIDI si el control está activo.
    if (!m_midiService || !m_currentMidiChannel || !m_isActive)
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