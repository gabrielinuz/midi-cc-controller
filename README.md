# MIDI CC Controller

## Descripción del Proyecto

**MIDI CC Controller** es una aplicación de escritorio configurable diseñada para controlar dispositivos MIDI utilizando mensajes de Controladores Continuos (CC). Desarrollada en C++ con la interfaz gráfica FLTK y la biblioteca RtMidi para la comunicación MIDI, esta herramienta permite a los músicos y productores configurar y manipular fácilmente los parámetros de sus equipos MIDI de hardware y software.

La aplicación destaca por su capacidad de cargar configuraciones de sliders "Midi Layouts" desde un archivo CSV y la funcionalidad para guardar y cargar "presets" de estados de los controladores de ese Layout cargado mediante otro archivo CSV.

El proyecto está diseñado con una arquitectura orientada a objetos modular, siguiendo los principios de diseño SOLID (especialmente el Principio Abierto/Cerrado y SRP, principio de responsabilidad única), lo que facilita su extensión.

## Características Principales

* **Controladores MIDI CC configurables**: Define sliders para cualquier número de CC MIDI (0-127) con rangos personalizados (mínimo/máximo).
* **Selección de Puerto MIDI**: Elige dinámicamente entre los puertos MIDI de salida disponibles en tu sistema.
* **Selector de Canal MIDI**: Controla todos los sliders en un canal MIDI específico (1-16).
* **Interfaz Gráfica Intuitiva (GUI)**: Basada en FLTK, ofrece una interfaz de usuario limpia y redimensionable para una experiencia cómoda.
* **Configuración por Archivo CSV**: Carga la definición de los sliders desde un archivo `config.csv` externo, permitiendo una personalización flexible sin recompilar el código.
* **Portabilidad (Linux)**: Incluye bibliotecas dinámicas (DLLs/SOs) en un directorio `bin` para facilitar la ejecución sin dependencias adicionales del sistema en GNU/Linux.

## Arquitectura del Proyecto

El proyecto sigue una arquitectura orientada a objetos (OOP) con una clara separación de responsabilidades para mejorar la modularidad, la mantenibilidad y la extensibilidad, adhiriéndose al Principio Abierto/Cerrado (OCP).

```text
.
├── bin/                       # Archivos binarios y compilados
│   ├── config-files-examples  # Ejemplos de configuraciones de sintetizadores.
│   ├── lib                    # Bibliotecas dinámicas necesarias para la ejecución (específico de Linux)
│       ├── libfltk.so.1.3     # Biblioteca FLTK
│       ├── librtmidi.so       # Biblioteca RtMidi
│   ├── config.csv             # Configuración inicial de la aplicación.
│   ├── execute.sh             # Script en bash para forzar la precarga de las bibliotecas locales.
│   ├── patch1.csv             # Patch de sonido de ejemplo. 
├── include/                   # Archivos de cabecera (.h/.hpp)
│   ├── vendors                # Directorio para bibliotecas de terceros.
│       ├── fltk               # Headers de la biblioteca gráfica FLTK
│       ├── rtmidi             # Headers de la biblioteca rtmidi
│   ├── Application.hpp        # Define la clase `Application`, el orquestador principal del ciclo de vida de la app.          
│   ├── MidiLayoutParser.hpp   # Define el `namespace MidiLayoutParse` para cargar layouts de dispositivos MIDI desde archivos CSV.
│   ├── MidiPresetParser.hpp   # Define el `namespace MidiPresetParse` para cargar presets de dispositivos MIDI desde archivos CSV.
│   ├── IMidiControl.hpp       # Define la interfaz abstracta `IMidiControl` para cualquier control MIDI de la GUI (favorece OCP).
│   ├── MainWindow.hpp         # Define la clase `MainWindow`, que gestiona la ventana principal y sus widgets.
│   ├── MidiService.hpp        # Define la clase `MidiService`, que encapsula toda la lógica de comunicación con RtMidi.
│   ├── SliderConfig.hpp       # Define la estructura `SliderConfig` para almacenar la configuración de un slider (CC#, descripción, rango). 
│   └── SliderControl.hpp      # Define la clase `SliderControl`, una implementación concreta de `IMidiControl` para sliders.
├── src/
│   ├── Application.cpp        # Implementa la lógica de `Application`, inicializando y conectando los componentes principales.  
│   ├── MidiLayoutParser.cpp   # Implementa las funciones de `MidiLayoutParser` para parsear los archivos de layouts CSV.      
│   ├── MidiPresetParser.cpp   # Implementa las funciones de `MidiPresetParser` para parsear los archivos de presets CSV.      
│   ├── main.cpp               # Contiene la función `main()`, el punto de entrada que crea y ejecuta la instancia de `Application`.
│   ├── MainWindow.cpp         # Implementa la lógica y el comportamiento de la interfaz de usuario de `MainWindow`.                 
│   ├── MidiService.cpp        # Implementa los detalles de la comunicación MIDI, utilizando la librería RtMidi.   
│   └── SliderControl.cpp      # Implementa la creación de widgets y el manejo de eventos para los sliders MIDI.
```





  