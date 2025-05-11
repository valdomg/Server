#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"

#include "Library/IRQ.h"
#include "Library/Joystick.h"

#include "hardware/clocks.h" 
#include "hardware/pll.h" 

const char WIFI_SSID [20] = "Online.Rosa";
const char WIFI_PASS [20] = "3914902323";

const char HOST [20] = "192.168.1.19";
const int PORT = 5000;

uint buffer_read_x = 0;
uint buffer_read_y = 0;

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

// Tenta enviar o primeiro item do buffer
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

    //sniprintf(path, sizeof(path), "");

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

int main()
{
    stdio_init_all();

    setupBTN();
    setupJoy();
    gpio_set_irq_enabled_with_callback(BTN_JOY, GPIO_IRQ_EDGE_RISE, true, &resetIRQ);

    // Inicializa Wi-Fi
    conectarWifi();

    int buffer_x, buffer_y = 0;

    while (true){

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

        //printf("Actual read:%d buffer_read %d\n", x, buffer_read_x);

        cyw43_arch_poll();
        sleep_ms(50);
    }
    
    return 0;
}
