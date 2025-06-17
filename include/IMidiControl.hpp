/**
 * @file IMidiControl.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Define la interfaz abstracta para cualquier control MIDI en la GUI.
 * @version 0.6
 * @date 2025-06-13
 * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * @link http://www.apache.org/licenses/LICENSE-2.0
 */
#pragma once

#include <FL/Fl_Widget.H>
#include <string>

/**
 * @class IMidiControl
 * @brief Interfaz (clase base abstracta) para controles MIDI.
 * @details Esta interfaz es la clave para cumplir con el Principio Abierto/Cerrado (OCP).
 * Cualquier nuevo tipo de control (un botón, un knob, etc.) puede ser añadido
 * creando una nueva clase que herede de esta, sin necesidad de modificar el
 * código existente de la ventana principal.
 * @version 0.4: Se añadieron métodos virtuales puros para gestionar el estado
 * del control (valor, CC#, etc.), permitiendo la funcionalidad de guardar/cargar patches.
 * @version 0.6: Se añaden métodos para gestionar el estado de activación del control.
 */
class IMidiControl 
{
    public:
        /**
        * @brief Destructor virtual por defecto. Esencial en clases base con funciones virtuales.
        */
        virtual ~IMidiControl() = default;

        /**
        * @brief Crea los widgets de FLTK necesarios para este control.
        * @pure
        * @param x La coordenada X donde se debe crear el control.
        * @param y La coordenada Y donde se debe crear el control.
        * @param w El ancho del control.
        * @param h La altura del control.
        * @param currentMidiChannel Un puntero al canal MIDI actual de la ventana principal.
        * Esto permite que el control siempre sepa en qué canal enviar
        * los mensajes sin sistemas de notificación complejos.
        */
        virtual void createWidgets(int x, int y, int w, int h, unsigned char* currentMidiChannel) = 0;

        /**
        * @brief Devuelve un puntero al widget principal o grupo de widgets del control.
        * @details La ventana principal usará este método para añadir el control a su jerarquía de widgets.
        * @pure
        * @return Fl_Widget* Un puntero al widget o grupo (Fl_Group) que encapsula este control.
        */
        virtual Fl_Widget* getWidgetGroup() = 0;

        /**
        * @brief Devuelve la altura total que ocupa el control en la GUI.
        * @details Esto es útil para que la ventana principal pueda calcular las posiciones
        * verticales de los controles de forma dinámica.
        * @pure
        * @return int La altura en píxeles.
        */
        virtual int getHeight() const = 0;

        // --- Nuevos métodos virtuales para la gestión de patches ---

        /**
         * @brief Obtiene el número de CC asociado a este control.
         * @pure
         * @return int El número de Control Change (0-127).
         */
        virtual int getCcNumber() const = 0;

        /**
         * @brief Obtiene la descripción textual de este control.
         * @pure
         * @return std::string La descripción del control.
         */
        virtual std::string getDescription() const = 0;
        
        /**
         * @brief Obtiene el rango de valores del control en formato de texto "min-max".
         * @pure
         * @return std::string El rango como texto.
         */
        virtual std::string getRange() const = 0;

        /**
         * @brief Obtiene el valor actual del control.
         * @pure
         * @return int El valor actual.
         */
        virtual int getCurrentValue() const = 0;

        /**
         * @brief Establece el valor actual del control.
         * @details Este método solo debe actualizar la GUI (ej. la posición del slider).
         * No debe disparar el envío de un mensaje MIDI.
         * @pure
         * @param value El nuevo valor a establecer.
         */
        virtual void setCurrentValue(int value) = 0;

        // --- @version 0.6: Métodos para el estado de activación ---

        /**
        * @brief Establece el estado de activación del control.
        * @details Un control inactivo no debería enviar mensajes MIDI y podría aparecer
        * visualmente deshabilitado.
        * @param active true para activar el control, false para desactivarlo.
        */
        virtual void setActive(bool active) = 0;

        /**
        * @brief Comprueba si el control está actualmente activo.
        * @return true si el control está activo, false en caso contrario.
        */
        virtual bool isActive() const = 0;
};