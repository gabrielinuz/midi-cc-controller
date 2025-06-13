#!/bin/bash

g++ \
-std=c++17 \
-Wall \
-ldl \
-I./include \
-I./include/vendors/fltk/include \
-I./include/vendors/rtmidi/include \
-I./lib/FL \
-L./include/vendors/fltk/lib/ \
-L./include/vendors/rtmidi/lib/ \
./src/Application.cpp \
./src/ConfigParser.cpp \
./src/MainWindow.cpp \
./src/MidiService.cpp \
./src/SliderControl.cpp \
./src/main.cpp \
./include/vendors/rtmidi/src/RtMidi.cpp \
-o ./bin/midi_cc_editor \
-D__LINUX_ALSA__ \
-DRTMIDI_API_ALSA=ON \
-DRTMIDI_API_JACK=OFF \
-DRTMIDI_API_PULSE=OFF \
-lfltk \
-lrtmidi \
-lasound \
-ldl \
-lpthread
