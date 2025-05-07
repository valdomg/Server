# 📡 Joystick Web Tracker com Raspberry Pi Pico W e Flask

Este projeto integra um joystick analógico com o microcontrolador **Raspberry Pi Pico W**, que envia coordenadas X e Y via Wi-Fi para um servidor **Flask**. Os dados são atualizados em tempo real em uma página web utilizando **Socket.IO**.

---

## 🔧 Requisitos

### Raspberry Pi Pico W
- SDK do Pico (C)
- Bibliotecas:
  - `cyw43_arch` (Wi-Fi)
  - `lwip` (HTTP)
  - Drivers para ADC e interrupções (GPIO)
- Conexão com joystick analógico (e.g. potenciômetros X/Y)
- Código C para leitura e envio das coordenadas

### Servidor Flask (Python)
- Python 3.7 ou superior
- Instale as dependências com:
  ```bash
  pip install flask flask-socketio

