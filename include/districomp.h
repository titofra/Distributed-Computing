/**
 * @brief Basic Distributed Computing's Implementation. Check out http://github.com/titofra/Distributed-Computing for more information
 * @author Titouan ABADIE - http://github.com/titofra - 04/23
*/

#ifndef DISTRICOMP_H
#define DISTRICOMP_H

#define N_CHR_TSK_MAX 4 +1   // number of characters for task's id (i.e. 3 means that tasks goes from 0 to 999). +1 for the end character ('0')

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "mailbox.h"
#include "mutex.h"
#include "webserver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _task {
    uint16_t id;
    char *result;
    const char* data_in;
    bool isDone;
} _task_t;

typedef struct _client {
    int sck;
    struct sockaddr_in addr;
    int tsk_id; // -1 if not working
} _client_t;

typedef struct districomp_srv {
    int mainSock;
    int tmp_cli_sck;
    struct sockaddr_in addr;
    _client_t* clients;
    uint16_t nb_cli;
    mutex_t clients_mut;
    _task_t* tasks;
    uint16_t nb_tsk;
    mutex_t tasks_mut;
    pthread_t listening_thrd;
    pthread_t receiving_thrd;
    mailbox_t waitingTasks;
    char* path_to_ws_tsk;
    webserver_srv_t webSrv;
} districomp_srv_t;

typedef struct districomp_cli {
    int srv_sck;
    int ws_sck;
    struct sockaddr_in srv_addr;
    struct sockaddr_in ws_addr;
    int domain;
    int type;
    int protocol;
} districomp_cli_t;

/**
 * @brief Initialise the districomp's server
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @param const uint16_t port, Port for the socket connection
 * @param const uint16_t ws_port, Port for the webserver connection
 * @param const int domain, Communication domain (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
 * @param const int type, Communication type (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
 * @param const int protocol, Communication protocol (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
 * @param const char* path_to_ws_root, Path to the webserver's root folder
 * @param const uint16_t maxSockReqQeued, Maximum number of pending connections to webserver (check backlog param at https://www.man7.org/linux/man-pages/man2/listen.2.html for more information)
*/
void InitServer (districomp_srv_t* srv, const uint16_t port, const uint16_t ws_port, const int domain, const int type, const int protocol, const char* path_to_ws_root, const uint16_t maxSockReqQeued);

/**
 * @brief Start a new thread which listen for incoming connection to the server
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @param const uint16_t maxSockReqQeued, Maximum number of pending connections (check backlog param at https://www.man7.org/linux/man-pages/man2/listen.2.html for more information)
*/
void StartListening (districomp_srv_t* srv, const uint16_t maxSockReqQeued);

/**
 * @brief Private function called by the thread in StartListening
*/
void* _Listening (void* arg);

/**
 * @brief Stop the thread that where listening for incoming connections (previously called by StartListening)
 * @param districomp_srv_t* srv, Pointer to the districomp's server
*/
void StopListening (districomp_srv_t* srv);

/**
 * @brief Start a new thread which listen for incoming messages
 * @param districomp_srv_t* srv, Pointer to the districomp's server
*/
void StartReceiving (districomp_srv_t* srv);

/**
 * @brief Private function called by the thread in StartReceiving
*/
void* _Receiving (void* arg);

/**
 * @brief Stop the thread that where listening for incoming messages (previously called by StartReceiving)
 * @param districomp_srv_t* srv, Pointer to the districomp's server
*/
void StopReceiving (districomp_srv_t* srv);

/**
 * @brief Add the task to the network. Task can be instantely executed by a client or can wait for its execution.
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @param const char* data, Data needed by the client to run the task
 * @return The task's id or -1 if the task cannot be added
*/
int AddTask (districomp_srv_t* srv, const char* data);

/**
 * @brief Private function that create a file on the webserver filled with the task's data
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @param uint16_t tsk_id, Task's id that must be added to the webserver
*/
void _AddDataWSFileTsk (districomp_srv_t* srv, uint16_t tsk_id);

/**
 * @brief Private function that send the task's id to the worker
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @param uint16_t cli_id, The worker's id
*/
void _SendTskCli (districomp_srv_t* srv, uint16_t cli_id);

/**
 * @brief Check if all the clients are waiting for a task/not working
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @return True if all the clients are sleeping, else false  
*/
bool AreSleeping (districomp_srv_t* srv);

/**
 * @brief Get the result of an executed task
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @param int task_id, Task's id
 * @return The string returned by the worker. Empty string ("") is returned if the task is not terminated/executed
*/
char* GetResult (districomp_srv_t* srv, int task_id);

/**
 * @brief Remove all the tasks. Note that executing tasks will not be stopped, however they will be remove
 * @param districomp_srv_t* srv, Pointer to the districomp's server
*/
void ClearTasks (districomp_srv_t* srv);

/**
 * @brief Get the number of workers
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @return The number of workers
*/
uint16_t GetNbClients (districomp_srv_t* srv);

/**
 * @brief Get the number of tasks
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @return The number of tasks
*/
uint16_t GetNbTasks (districomp_srv_t* srv);

/**
 * @brief Get the number of pending or running tasks aka non-terminated tasks
 * @param districomp_srv_t* srv, Pointer to the districomp's server
 * @return The number of remaining tasks
*/
uint16_t GetNbRemainingTasks (districomp_srv_t* srv);

/**
 * @brief Close the districomp's server
 * @param districomp_srv_t* srv, Pointer to the districomp's server
*/
void CloseServer (districomp_srv_t* srv);

/**
 * @brief Wait until Listening and Receiving threads ends
 * @param districomp_srv_t* srv, Pointer to the districomp's server
*/
void WaitForThreadsEnd (districomp_srv_t* srv);

/**
 * @brief Initialise a districomp's client
 * @param districomp_cli_t* cli, Pointer to the districomp's client
 * @param const char* addr, Address of the server
 * @param const uint16_t sock_port, Port for the socket connection
 * @param const int domain, Communications domain (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
 * @param const int type, Communications type (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
 * @param const int protocol, Communications protocol (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
 * @param const uint16_t ws_port, Port to the webserver
*/
void InitClient (districomp_cli_t* cli, const char* addr, const uint16_t sock_port, const int domain, const int type, const int protocol, const uint16_t ws_port);

/**
 * @brief Wait for a task given by the server, then provide the data for executing this task. Note that this is a blocking function
 * @param districomp_cli_t* cli, Pointer to the districomp's client
 * @param char** result, Pointer to a string that will be the data for executing the task
*/
void ReceiveTask (districomp_cli_t* cli, char** result);

/**
 * @brief Send the result of the executed task to the server
 * @param districomp_cli_t* cli, Pointer to the districomp's client
 * @param const char* data, The result of the task
*/
void SendResult (districomp_cli_t* cli, const char* data);

/**
 * @brief Close a districomp's client
 * @param districomp_cli_t* cli, Pointer to the districomp's client
*/
void CloseClient (districomp_cli_t* cli);

#ifdef __cplusplus
}
#endif

#endif  // DISTRICOMP_H
