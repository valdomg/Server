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

const char WIFI_SSID [20] = "Online.Rosa";
const char WIFI_PASS [20] = "3914902323";

const char HOST [20] = "192.168.1.19";
const uint PORT = 80;


#define BUFFER_SIZE 10

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
void enviar_do_buffer() {
    if (buffer_inicio == buffer_fim) {
        // buffer vazio
        return;
    }

    Coordenada *c = &buffer[buffer_inicio];

    char path[100];
    snprintf(path, sizeof(path), "/?x=%u&y=%u", c->x, c->y);

    EXAMPLE_HTTP_REQUEST_T req = {0};
    req.hostname = HOST;
    req.url = path;
    req.port = PORT;
    req.headers_fn = http_client_header_print_fn;
    req.recv_fn = http_client_receive_print_fn;

    printf("Enviando: %s\n", path);
    int result = http_client_request_sync(cyw43_arch_async_context(), &req);

    if (result == 0) {
        printf("Enviado com sucesso!\n");
        buffer_inicio = (buffer_inicio + 1) % BUFFER_SIZE; // remove do buffer
    } else {
        printf("Erro ao enviar (%d), tentando novamente depois.\n", result);
        // Não avança o buffer — tentativa futura
    }

    sleep_ms(100);
}

int main()
{
    stdio_init_all();

    setupBTN();
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_RISE, true, &resetIRQ);
    setupJoy();
    // Inicializa Wi-Fi

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
    
    
    while (true){
        
        uint x = read_adc_x();
        uint y = read_adc_y();

        adicionar_ao_buffer(x, y);

        enviar_do_buffer();

        sleep_ms(1000);

    }
    
}
