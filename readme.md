# 🧭 Projeto Rosa dos Ventos com Raspberry Pi Pico W

Este projeto integra um **joystick analógico** conectado ao **Raspberry Pi Pico W** com uma **interface web em tempo real**. As informações de posição (coordenadas X e Y) e os estados dos botões são enviados via HTTP para um servidor Flask, que os repassa para o navegador usando WebSockets (Socket.IO).

---

## 📦 Arquivos do Projeto

### 🧠 Microcontrolador (C/C++ para Raspberry Pi Pico W)
- **`Server.c`**  
  Código principal que:
  - Lê o joystick e botões físicos.
  ```c
   uint x = read_adc_x();
        uint y = read_adc_y();

        uint btn_a = !gpio_get(BTN_A);
        uint btn_b = !gpio_get(BTN_B);

        
        if(btn_a || btn_b ){   
            requestBtn(btn_a, btn_b);
        }


        if (buffer_x != x && buffer_y != y)
        {
            buffer_x = x;
            buffer_y = y;

            adicionar_ao_buffer(x, y);
            enviar_request();
        }
  
  ```

  - Armazena coordenadas em um buffer circular.
  ```c
      #define BUFFER_SIZE 5

      typedef struct {
          uint x;
          uint y;
          bool enviado;
      } Coordenada;

      Coordenada buffer[BUFFER_SIZE];
      int buffer_inicio = 0;
      int buffer_fim = 0;

      // Adiciona coordenadas ao buffer
      void adicionar_ao_buffer(uint x, uint y) {
          int proximo = (buffer_fim + 1) % BUFFER_SIZE;
          if (proximo != buffer_inicio) { // Se não estiver cheio
              buffer[buffer_fim].x = x;
              buffer[buffer_fim].y = y;
              buffer[buffer_fim].enviado = false;
              buffer_fim = proximo;
          } else {
              printf("Buffer cheio! Descartando coordenada x=%u y=%u\n", x, y);
          }
      }

  ```

  - Envia as informações via HTTP ao servidor Flask.
  ```c
    void enviar_request() {

        if (buffer_inicio == buffer_fim) {
            // buffer vazio
            return;
        }

        Coordenada *c = &buffer[buffer_inicio];
        int x = c->x;
        int y = c->y; 

        char path[30];
        
        snprintf(path, sizeof(path), "/coordenadas?x=%d&y=%d", x, y);

        EXAMPLE_HTTP_REQUEST_T req = {0};
        req.hostname = HOST;
        req.url = path;
        req.port = PORT;

        printf("Enviando: %s\n", path);
        int result = http_client_request_sync(cyw43_arch_async_context(), &req);
        printf("%d ", result);

        if (result == 0) {
            printf("Enviado com sucesso!\n");
            buffer_inicio = (buffer_inicio + 1) % BUFFER_SIZE; // remove do buffer
        } else {
            printf("Erro ao enviar (%d), tentando novamente depois.\n", result);
            
        }
    }

    void requestBtn(uint btn_direito, uint btn_esquerdo){

      char path_btn[30];
      
      snprintf(path_btn, sizeof(path_btn), "/btn?a=%d&b=%d", btn_direito, btn_esquerdo);

      EXAMPLE_HTTP_REQUEST_T req = {0};
      req.hostname = HOST;
      req.url = path_btn;
      req.port = PORT;


      printf("Enviando: %s\n", path_btn);
      int result = http_client_request_sync(cyw43_arch_async_context(), &req);
      printf("%d ", result);

      if (result == 0) {
          printf("Enviado com sucesso!\n");
      } else {
          printf("Erro ao enviar (%d), tentando novamente depois.\n", result);
          
      }
  }
  ```

  - Gerencia conexão Wi-Fi e requisições.
  ```c
    void conectarWifi(){

      if (cyw43_arch_init()){
          printf("Erro ao inicializar o Wi-Fi\n");
      }

      cyw43_arch_enable_sta_mode();
      printf("Conectando ao Wi-Fi...\n");

      if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)){
          printf("Falha ao conectar ao Wi-Fi\n");  
      }

      else{
          uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
          printf("Wi-Fi conectado! IP: %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]); 
      }
  }
  
  ```


- **`Library/IRQ.h`**  
  Define os pinos dos botões e a interrupção para reset USB via botão do joystick.
  ```c
    #include "pico/stdio.h"
    #include "pico/stdlib.h"
    #include "pico/bootrom.h"

    const int BTN_A = 5;

    const int BTN_B = 6;

    const int BTN_JOY = 22;

    void setupBTN(){
        gpio_init(BTN_A);
        gpio_set_dir(BTN_A, GPIO_IN);
        gpio_pull_up(BTN_A);

        gpio_init(BTN_B);
        gpio_set_dir(BTN_B, GPIO_IN);
        gpio_pull_up(BTN_B);

        gpio_init(BTN_JOY);
        gpio_set_dir(BTN_JOY, GPIO_IN);
        gpio_pull_up(BTN_JOY);

    }



    void resetIRQ(uint gpio, uint32_t events){
        reset_usb_boot(0, 0);
    }


  ```

- **`Library/Joystick.h`**  
  Inicializa o ADC e lê os valores dos eixos X e Y do joystick.
```c
  #include "hardware/adc.h"
  #include "pico/stdlib.h"

  const int ADC_1 = 26;
  const int ADC_2 = 27;

  const int X = 1;
  const int Y = 0;

  void setupJoy(){

      adc_init();

      //Configuração dos Pinos como entradas ADC(alta impedância, dificultando a passagem elétrica)
      //sem resistores pull_up
      adc_gpio_init(ADC_1);
      adc_gpio_init(ADC_2);


  }

  int read_adc_x(){
      adc_select_input(X);
      uint adc_x_raw = adc_read();

      return adc_x_raw;
  }

  uint read_adc_y(){
      adc_select_input(Y);
      uint adc_y_raw = adc_read();

      return adc_y_raw;
  }
```
---

### 🌐 Interface Web (HTML + JS)
- **`templates/index.html`**  
  Página principal que:
  - Exibe uma "Rosa dos Ventos".
  - Mostra a posição do joystick como um ponteiro flutuante.
  - Altera a cor do ponteiro com base nos botões pressionados.
  - Recebe eventos em tempo real via WebSocket (`socket.io`).

---

### 🖥️ Backend (Python Flask + Socket.IO)
- **`app.py`**  
  Servidor web que:
  - Renderiza a interface em `/`.
  - Recebe coordenadas via `/coordenadas?x=...&y=...`.
  - Recebe estados dos botões via `/btn?a=...&b=...`.
  - Emite eventos WebSocket para atualizar a interface em tempo real.

---

## 🛠️ Tecnologias Utilizadas

- **Microcontrolador:** Raspberry Pi Pico W
- **Linguagem de Firmware:** C/C++ (SDK do Pico)
- **Servidor Web:** Flask + Flask-SocketIO
- **Frontend:** HTML, CSS, JavaScript (Socket.IO)

---

## 🚀 Como Funciona

1. O Pico W conecta ao Wi-Fi e começa a ler os valores do joystick e dos botões.
2. Sempre que os valores mudam:
   - Envia coordenadas para o servidor (`/coordenadas`).
   - Envia estado dos botões (`/btn`).
3. O servidor Flask recebe os dados e os transmite para o navegador via WebSocket.
4. A interface web atualiza em tempo real a posição e cor do ponteiro.

---

## 📡 Comunicação

| Origem         | Destino        | Protocolo | Descrição                             |
|----------------|----------------|-----------|-----------------------------------------|
| Pico W         | Servidor Flask | HTTP      | Envia coordenadas e botões             |
| Servidor Flask | Navegador Web  | WebSocket | Emite eventos com os dados recebidos   |

---
## Organização do repositório
```c
📁 SERVER/
├── server.c
├── example_http_client_util.c
├── example_http_client_util.h
├── lwipopts_examples_common.h
├── mdebtls_config_examples_common.h
├── mdebtls_config.h
├── lwipopts
├── Library/
│   ├── Joystick.h
│   └── IRQ.h
├── app.py
├── templates/
│   └── index.html
├── WebPage/
│   ├── static/
│       ├── css/
│       │ └── style.css  
│       ├── img/
│       │ └── rosa.png
│       ├── socket.io.min.js
│   ├── templates/
│       ├── index.html
│   ├──page.py
```` 
## ⚙️ Executando o Projeto

### 1. Instale as dependências do servidor
```bash
pip install flask
```
```bash
pip install flask flask-socketio
```
## 2. Execute o arquivo page.py
```bash
cd ./WebPage/
```
```bash
py page.py
```

## 3. Copie o endereço que será informado no terminal
```bash
Ex: 192.168.X.XX
```

## 4. Cole o endereço no arquivo Server.c em HOST
```bash
ex: const char HOST [20] = "192.168.X.XX";
```

## 3. Informe o nome da Rede e a Senha no arquivo Server.c
```bash
ex: const char WIFI_SSID [20] = "12343456";
ex: const char WIFI_PASS [20] = "00000000";
```