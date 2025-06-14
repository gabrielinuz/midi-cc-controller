/**
 * @file SliderControl.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Implementación concreta de un control MIDI de tipo Slider.
 * @version 0.4
 * @date 2025-06-13
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */
#pragma once

#include "IMidiControl.hpp"
#include "SliderConfig.hpp"
#include "MidiService.hpp"
#include <FL/Fl_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Output.H>
#include <memory>
#include <string> // Necesario para std::string

/**
 * @class SliderControl
 * @brief Implementa la interfaz IMidiControl para un slider que envía mensajes CC.
 * @details Esta clase gestiona la creación de un label y un slider de FLTK,
 * y maneja el callback para enviar mensajes MIDI a través del MidiService.
 * @version 0.5: Se implementan los nuevos métodos virtuales de IMidiControl.
 */
class SliderControl : public IMidiControl 
{
    public:
        /**
        * @brief Construye un nuevo objeto SliderControl.
        * @param config La configuración (CC#, descripción, rango) para este slider.
        * @param midiService Un puntero compartido al servicio MIDI para enviar mensajes.
        */
        SliderControl(const SliderConfig& config, std::shared_ptr<MidiService> midiService);

        /** @copydoc IMidiControl::createWidgets() */
        void createWidgets(int x, int y, int w, int h, unsigned char* currentMidiChannel) override;
        
        /** @copydoc IMidiControl::getWidgetGroup() */
        Fl_Widget* getWidgetGroup() override;

        /** @copydoc IMidiControl::getHeight() */
        int getHeight() const override;

        // --- Implementación de los nuevos métodos de la interfaz ---

        /** @copydoc IMidiControl::getCcNumber() */
        int getCcNumber() const override;

        /** @copydoc IMidiControl::getDescription() */
        std::string getDescription() const override;

        /** @copydoc IMidiControl::getRange() */
        std::string getRange() const override;

        /** @copydoc IMidiControl::getCurrentValue() */
        int getCurrentValue() const override;

        /** @copydoc IMidiControl::setCurrentValue(int) */
        void setCurrentValue(int value) override;
        
        /**
        * @brief Callback estático que FLTK puede invocar.
        * @param w El widget que originó el callback.
        * @param userdata Un puntero a la instancia de SliderControl.
        */
        static void sliderCallback_static(Fl_Widget* w, void* userdata);

       /**
        * @brief Obtiene el puntero al widget Fl_Slider interno.
        * @return Fl_Slider* El puntero al widget Fl_Slider.
        */
        Fl_Slider* getFlSlider() const { return m_slider; }

    private:
        /**
        * @brief Método de instancia que contiene la lógica real del callback.
        */
        void sliderCallback();

        /// @brief Configuración específica para este slider.
        SliderConfig m_config;

        /// @brief Puntero compartido al servicio MIDI.
        std::shared_ptr<MidiService> m_midiService;

        /// @brief Puntero al canal MIDI actual, propiedad de MainWindow.
        unsigned char* m_currentMidiChannel;

        /// @brief Almacena el texto del tooltip para asegurar que el puntero sea válido durante la vida del widget.
        std::string m_tooltipText;

        // --- Widgets de FLTK ---
        Fl_Group* m_group;      ///< Un grupo para mantener juntos la etiqueta y el slider.
        Fl_Box* m_label;        ///< La etiqueta descriptiva del slider.
        Fl_Slider* m_slider;    ///< El slider interactivo.
        Fl_Value_Output* m_valueOutput; ///< Widget para mostrar el valor del slider.
};