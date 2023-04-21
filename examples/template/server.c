#include <districomp.h>

#define PORT 8080
#define WS_PORT 1818

int main (void) {
    districomp_srv_t srv;

    // Initialise tcp socket server
    InitServer (&srv, PORT, WS_PORT, AF_INET, SOCK_STREAM, 0, "./webserver_root/", 3);

    // Start listening for incoming connection
    StartListening (&srv, 128);

    // Start for incoming message
    StartReceiving (&srv);

    // Add some tasks
    int tsk0_id = AddTask (&srv, "This is the data for the first task !");
    int tsk1_id = AddTask (&srv, "{id: 1, data: 61518}");
    int tsk2_id = AddTask (&srv, "Well, we can put anything as data\nEverything, almost...");

    // Wait until there is a still a tasks
    while (GetNbRemainingTasks (&srv) > 0) {
        // Some tasks either are still waiting to be executed or are executing... 
    }

    // Process results
    char* res_task0 = GetResult (&srv, tsk0_id);
    char* res_task1 = GetResult (&srv, tsk1_id);
    char* res_task2 = GetResult (&srv, tsk2_id);
    printf ("1: %s\n2: %s\n3: %s\n", res_task0, res_task1, res_task2);

    // Close everything
    CloseServer (&srv);

    return 0;
}