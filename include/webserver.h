/*
    @brief This is a Basic Webserver
    @author Titouan ABADIE - http://github.com/titofra - 04/23
*/

#ifndef WEBSERVER_H
#define WEBSERVER_H

#define MAX_LEN_INCOMING_DATA 1024
#define MAX_LEN_FILES 1024
#define N_CHR_TSK_MAX 4 +1   // number of characters for task's id (i.e. 3 means that tasks goes from 0 to 999). +1 for the end character ('0')

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct webserver_srv {
    int sck;
    int tmp_cli_sck;
    struct sockaddr_in addr;
    const char* path_to_ws_root;
    pthread_t listening_thrd;
} webserver_srv_t;

/*
    @brief Initialise the webserver
    @param webserver_srv_t* srv, Pointer to the webserver
    @param const uint16_t port, Port of the webserver
    @param const int domain, Communication domain (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
    @param const int type, Communication type (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
    @param const int protocol, Communication protocol (check https://www.man7.org/linux/man-pages/man2/socket.2.html for more information)
    @param const char* path_to_ws_root, Path to the webserver's root folder
*/
void InitWebServer (webserver_srv_t* srv, const uint16_t port, const int domain, const int type, const int protocol, const char* path_to_ws_root);

/*
    @brief Start a new thread which listen for incoming connection to the webserver
    @param webserver_srv_t* srv, Pointer to the webserver
    @param const uint16_t maxSockReqQeued, Maximum number of pending connections (check backlog param at https://www.man7.org/linux/man-pages/man2/listen.2.html for more information)
*/
void StartWSListening (webserver_srv_t* srv, const uint16_t maxSockReqQeued);

/*
    @brief Private function called by the thread in StartListening
*/
void* _WSListening (void* arg);

/*
    @brief Stop the thread that where listening for incoming connections (previously called by StartListening)
    @param webserver_srv_t* srv, Pointer to the districomp's server
*/
void StopWSListening (webserver_srv_t* srv);

/*
    @brief Private function that send the requested page
    @param int cli_sck, Client socket
    @param const char *filePath, Path to the requested file
*/
void _Send (int cli_sck, const char *filePath);

/*
    @brief Close the webserver
    @param webserver_srv_t* srv, Pointer to the webserver
*/
void CloseWebServer (webserver_srv_t* srv);

/*
    @brief Wait until Listening thread's end
    @param webserver_srv_t* srv, Pointer to the webserver
*/
void WSWaitForThreadsEnd (webserver_srv_t* srv);

#ifdef __cplusplus
}
#endif

#endif  // WEBSERVER_H
