#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "addr_from_stdin.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#define PORT CONFIG_EXAMPLE_PORT

static const char *TAG = "example";
float array[375] = {-20.0056, 11.3757, -15.7691, -20.0056, 7.3746, -17.4558, -17.8481, 9.2967, -17.4951, -3.3735, 14.9061, -11.4149, 4.2365, 14.3569, -5.8840, 20.0056, 11.0227, 6.5508, 20.0056, 8.8652, 14.2785, 20.0056, 3.0597, 15.5730, 13.4547, 8.0415, 5.7663, 5.5702, 10.7481, -3.7658, -13.2586, 11.1011, -20.0056, -18.3973, 11.2188, -20.0056, -20.0056, 9.2575, -8.1199, -20.0056, 8.3553, -16.0045, -10.2381, 14.7492, -14.5923, -1.2553, 15.4553, 1.6867, 12.3956, 15.3376, 4.0011, 20.0056, 4.4718, 15.5730, 20.0056, 3.9619, 18.0050, 11.3365, 7.6100, 7.9238, 4.3542, 9.3359, -1.4906, -3.0204, 7.2961, -17.9266, -20.0056, 6.1586, -3.3343, -20.0056, 11.2188, -7.1000, -20.0056, 15.1807, -15.2199, -15.4553, 18.2404, -15.3376, -0.5492, 14.3177, -17.2989, 20.0056, 10.7873, 1.0983, 20.0056, 5.2171, 7.8845, 20.0056, 3.0204, 11.7680, 20.0056, 6.6685, 10.2774, 6.3547, 13.4155, -3.7658, -4.5895, 15.1022, -13.0232, -20.0056, 16.2790, -20.0056, -20.0056, 11.8072, -8.7475, -20.0056, 9.6105, -10.4343, -19.8879, 14.5923, -19.1426, -6.0801, 17.8089, -15.7691, 13.2194, 13.9647, -0.6276, 20.0056, 8.7868, 4.2365, 20.0056, 0.7061, 14.8669, 19.6525, 5.3740, 9.8851, 13.1017, 8.3945, 2.2359, -6.1193, 9.6105, -17.2989, -18.5150, 8.1199, -20.0056, -20.0056, 10.3950, -7.2961, -20.0056, 12.5525, -12.2779, -20.0056, 15.9260, -19.9271, 0.6276, 18.9072, -10.6696, 9.1006, 18.4365, -5.4917, 20.0056, 7.9630, 3.6873, 20.0056, 1.9221, 9.6497, 20.0056, 6.1586, 14.2000, 14.0431, 11.3365, 0.0000, 4.3934, 12.9840, -9.2575, -20.0056, 10.9442, -20.0056, -20.0056, 11.2188, -16.5929, -20.0056, 12.1995, -7.2177, -20.0056, 14.3177, -11.5326, -13.3763, 18.7111, -17.0636, 10.2774, 16.9851, -4.9033, 19.4172, 11.9249, -2.9028, 20.0056, 0.3923, 7.6884, 20.0056, -0.7845, 11.6111, 19.9271, 6.4724, 5.6486, 12.7879, 10.0028, -3.6088, -19.0249, 11.1404, -20.0056, -20.0056, 13.6116, -9.5321, -20.0056, 11.9249, -5.4917, -20.0056, 19.9271, -19.2210, -20.0056, 20.0056, -20.0056, 2.2359, 20.0056, -19.7702, 15.2199, 9.8067, -8.5906, 20.0056, 4.7072, 2.4713, 20.0056, -0.5884, 8.0022, 20.0056, -0.6276, 6.1586, 16.3967, 8.1591, 0.8630, 5.2956, 10.5912, -5.2171, -20.0056, 15.2199, -13.6509, -20.0056, 16.2398, -2.9812, -20.0056, 14.6707, -3.5696, -20.0056, 20.0056, -10.4343, -12.1210, 20.0056, -11.0227, 14.2785, 15.3376, -6.7470, 10.5912, 7.3354, -10.0420, 20.0056, -1.0591, 4.8641, 20.0056, 1.0199, 6.5116, 20.0056, 7.9238, 4.6287, 8.0415, 17.6912, -8.9044, -9.7282, 20.0056, -14.7100, -20.0056, 20.0056, -20.0056, -20.0056, 15.9652, -13.9254, -20.0056, 16.3967, -14.9846, -20.0056, 20.0056, -19.0249, -8.1984, 20.0056, -16.5929, 14.7492, 14.5923, -6.7077, 20.0056, 9.4536, -5.1387, 20.0056, -2.5497, 3.4519, 20.0056, 3.4519, 9.3359, 20.0056, 7.4138, 6.4332, 7.3354, 15.6514, -5.0210, -6.0017, 20.0056, -10.8265, -20.0056, 20.0056, -19.6918, -20.0056, 17.5343, -19.4956, -20.0056, 17.5735, -4.1972, -20.0056, 20.0056, -15.0238, -12.5525, 20.0056, -13.1801, 13.3763, 19.4956, -7.2961, 20.0056, 9.7282, -11.3365, 20.0056, -7.5707, -5.5702, 20.0056, -8.0807, 5.6094, 20.0056, 7.4923, 7.2569, 15.1807, 20.0056, -6.7470, 0.8630, 20.0056, -13.9254, -20.0056, 20.0056, -20.0056, -20.0056, 20.0056, -20.0056, -20.0056, 15.0630, -2.0790, -20.0056, 19.6133, -11.6503, -15.2984, 20.0056, -18.0835, 16.5929, 16.9851, -10.5520, 20.0056, 8.3553, -7.3746, 20.0056, -8.1199, 4.2365, 20.0056, -7.4138, 10.8658, 20.0056, 5.2564, 5.3740, 2.3928, 13.8078, -12.1602};

static void tcp_client_task(void *pvParameters)
{
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        for (int i=0; i<125;i++) {

            float arr[4] = {1, array[i*3], array[i*3+1], array[i*3+2]};
            const char* payload = reinterpret_cast<char*>(arr);

            int err = send(sock, payload, 16, 0);
            if (err < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                break;
            }

            int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recv failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);
            }

            vTaskDelay(25 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, NULL);
}
