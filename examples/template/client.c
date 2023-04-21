#include <districomp.h>

#define SRV_PORT 8080
#define WS_PORT 1818
#define ADDR "127.0.0.1"

int main (void) {
    districomp_cli_t cli;

    // Initialise tcp socket with the server
    InitClient (&cli, ADDR, SRV_PORT, AF_INET, SOCK_STREAM, 0, WS_PORT);

    char* data = NULL;
    while (1) {
        // Wait for a new task
        ReceiveTask (&cli, &data);

        // Process with data
        const char* result = "this is the result !";

        // Send the result
        SendResult (&cli, result);

    }
    free (data);

    // Close everything
    CloseClient (&cli);

    return 0;
}
