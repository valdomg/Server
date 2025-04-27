#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>
#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"

#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"

#include "Library/IRQ.h"

const char WIFI_SSID [20] = "Online.Rosa";
const char WIFI_PASS [20] = "3914902323";

const char HOST [20] = "Endereço_do_server";

int main()
{
    stdio_init_all();

    setupBTN();
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_RISE, true, &resetIRQ);

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
    
    

    while (true) {
        sleep_ms(2000);
    }
}
