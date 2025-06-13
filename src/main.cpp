/**
 * @file main.cpp
 * @author Gabriel Nicolás González Ferreira (gabrielinuz@fi.mdp.edu.ar)
 * @brief archivo de la función principal de entrada de la aplicación
 * @version 0.3
 * @date 2025-06-12
 * * @copyright Copyright (c) 2025. This project is released under the Apache License.
 * * @link http://www.apache.org/licenses/LICENSE-2.0
 * */

#include "Application.hpp"

int main(int argc, char** argv) 
{
    auto app = std::make_unique<Application>();
    return app->run(argc, argv);
}