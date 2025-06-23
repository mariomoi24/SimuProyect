# SimuProyect
Para reto de programación de Simulacion

# 📚 Lectura de archivos JSON en C++ usando `nlohmann/json`

Este documento explica cómo leer archivos `.json` en C++ utilizando la popular biblioteca [nlohmann/json](https://github.com/nlohmann/json).

---

## 🧱 Requisitos

- Un compilador compatible con C++11 o superior (ej. `g++`, `clang`, `MSVC`)
- La biblioteca de cabecera `json.hpp` de `nlohmann`
- Archivo `.json` con estructura válida

---

## 📁 Estructura del proyecto recomendada

SimuProyect/
├── runmlaberintos.cpp # Tu programa principal en C++
├── laberinto10X10.json # Archivo JSON con el laberinto
├── json.hpp # Archivo de la librería nlohmann/json
├── runmlaberintos.exe # (Se genera tras compilar)
└── .gitignore
