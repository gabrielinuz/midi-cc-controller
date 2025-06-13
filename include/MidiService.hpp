/**
 * @file MidiService.hpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief Encapsula toda la lógica de comunicación MIDI utilizando la librería RtMidi.
 * @version 0.3
 * @date 2025-06-12
 * * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * * @link http://www.apache.org/licenses/LICENSE-2.0
 * */
#pragma once

#include "RtMidi.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @class MidiService
 * @brief Gestiona la comunicación MIDI de salida.
 * @details Esta clase actúa como un 'wrapper' o fachada para la librería RtMidi.
 * Su propósito es aislar el resto de la aplicación de los detalles de bajo
 * nivel de la API de RtMidi. Sigue el Principio de Responsabilidad Única (SRP),
 * enfocándose exclusivamente en la lógica MIDI.
 */
class MidiService 
{
    public:
        /**
        * @brief Construye un nuevo objeto MidiService.
        * @details Intenta inicializar una instancia de RtMidiOut. Si falla, almacena el
        * mensaje de error, que puede ser recuperado con getInitializationError().
        */
        MidiService();

        /**
        * @brief Destruye el objeto MidiService.
        * @details Se asegura de que el puerto MIDI esté cerrado antes de la destrucción.
        */
        ~MidiService();

        // Se deshabilita la copia para prevenir la gestión múltiple de un mismo recurso RtMidi.
        MidiService(const MidiService&) = delete;
        MidiService& operator=(const MidiService&) = delete;

        /**
        * @brief Abre un puerto MIDI de salida específico.
        * * @param portNumber El índice del puerto a abrir, basado en la lista de getPortNames().
        * @return true Si el puerto se abrió con éxito.
        * @return false Si ocurrió un error o el puerto ya estaba abierto.
        */
        bool openPort(unsigned int portNumber);

        /**
        * @brief Cierra el puerto MIDI si está abierto.
        */
        void closePort();

        /**
        * @brief Comprueba si un puerto MIDI está actualmente abierto.
        * * @return true Si hay un puerto abierto y listo para enviar mensajes.
        * @return false En caso contrario.
        */
        bool isPortOpen() const;

        /**
        * @brief Obtiene el número de puertos MIDI de salida disponibles.
        * * @return unsigned int El número de puertos encontrados.
        */
        unsigned int getPortCount() const;

        /**
        * @brief Obtiene el nombre de un puerto MIDI específico.
        * * @param portNumber El índice del puerto.
        * @return std::string El nombre del puerto.
        */
        std::string getPortName(unsigned int portNumber) const;

        /**
        * @brief Envía un mensaje MIDI de Control Change (CC).
        * * @param channel El canal MIDI (0-15).
        * @param cc El número de Control Change (0-127).
        * @param value El valor del Control Change (0-127).
        */
        void sendCcMessage(unsigned char channel, unsigned char cc, unsigned char value);

        /**
        * @brief Devuelve un mensaje de error si la inicialización de RtMidi falló.
        * * @return std::string El mensaje de error, o una cadena vacía si no hubo error.
        */
        std::string getInitializationError() const { return m_errorString; }

    private:
        /// @brief Puntero inteligente a la instancia de RtMidiOut. La propiedad es única de esta clase.
        std::unique_ptr<RtMidiOut> m_midiOut;
        
        /// @brief Almacena un mensaje de error si la construcción falla.
        std::string m_errorString;
};