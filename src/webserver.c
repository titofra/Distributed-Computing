#include "webserver.h"

void InitWebServer (webserver_srv_t* srv, const uint16_t port, const int domain, const int type, const int protocol, const char* path_to_ws_root) {
    // Create socket
    srv->sck = socket (domain, type, protocol);
    if (srv->sck < 0) {
        perror ("\nCan't create the socket\n");
        exit (1);
    }

    // Forcefully attaching the socket to the port
    int opt = 1;
    if (setsockopt(
        srv->sck,
        SOL_SOCKET,
        SO_REUSEADDR | SO_REUSEPORT,
        &opt,
        sizeof(opt))
    ){
        perror ("\nError: setsockopt failed\n");
        exit(1);
    }

    // Set port/ip
    srv->addr.sin_family = (sa_family_t) domain;    // may do some conversion error
    srv->addr.sin_port = htons(port);
    srv->addr.sin_addr.s_addr = INADDR_ANY;

    // Bind to port/ip
    if (bind (srv->sck, (struct sockaddr*)&srv->addr, sizeof(srv->addr)) < 0){
        perror ("\nCouldn't bind the socket to the port\n");
        exit (1);
    }

    // Init
    srv->path_to_ws_root = path_to_ws_root;
}

void StartWSListening (webserver_srv_t* srv, const uint16_t maxSockReqQeued) {
    // Listen for clients
    if (listen (srv->sck, maxSockReqQeued) < 0){
        perror ("\nCan't listen the socket\n");
        exit (1);
    }

    // Create the listening thread
    pthread_create(&srv->listening_thrd, NULL, &_WSListening, srv);
}

void* _WSListening (void* arg) {
    webserver_srv_t* srv = (webserver_srv_t*) arg;

    struct sockaddr_in cli_addr;
    socklen_t cli_size = sizeof(cli_addr);
    while (1) {
        // Accept an incoming connection
        srv->tmp_cli_sck = accept(srv->sck, (struct sockaddr*)&cli_addr, &cli_size);
        if (srv->tmp_cli_sck > 0) {
            // Read incoming request
            char buffer[MAX_LEN_INCOMING_DATA] = {0};
            read(srv->tmp_cli_sck, buffer, MAX_LEN_INCOMING_DATA);

            // Extract requested file path
            char* file_path = strtok (buffer, " ");
            file_path = strtok (NULL, " "); // pass the GET
            if (file_path != NULL) {
                // Build the file path
                char result [strlen (srv->path_to_ws_root) + N_CHR_TSK_MAX];
                strcpy (result, srv->path_to_ws_root);
                strcat (result, file_path);

                // Serve requested file
                _Send (srv->tmp_cli_sck, result);
            }
        } else {
            printf ("\n[Webserver] Can't accept the client at IP %s:%i\n",
                inet_ntoa(cli_addr.sin_addr),
                ntohs(cli_addr.sin_port)
            );
        }

        close (srv->tmp_cli_sck);
    }

}

void StopWSListening (webserver_srv_t* srv) {
    close (srv->tmp_cli_sck);   // close the temporary socket
    pthread_cancel (srv->listening_thrd);
}

void _Send (int cli_sck, const char *filePath) {

    int fd = open (filePath, O_RDONLY);
    if (fd < 0) {
        const char* error_message = "HTTP/1.1 404 Not Found\nContent-Length: 0\r\n\r\n";
        write (cli_sck, error_message, strlen(error_message));
    } else {
        struct stat file_stat;
        if (fstat (fd, &file_stat) < 0) {
            perror ("fstat failed");
            exit (EXIT_FAILURE);
        }
        const char* success_message = "HTTP/1.1 200 OK\nContent-Type: text/plain\r\n\r\n";
        char content_length[1024];
        sprintf (content_length, "Content-Length: %ld\n\n", file_stat.st_size);
        write (cli_sck, success_message, strlen (success_message));
        write (cli_sck, content_length, strlen (content_length));
        ssize_t read_size;
        char buffer[MAX_LEN_FILES] = {0};
        while ((read_size = read (fd, buffer, sizeof (buffer))) > 0) {
            write (cli_sck, buffer, (size_t) read_size);
        }
    }

    close (fd);
}

void CloseWebServer (webserver_srv_t* srv) {
    StopWSListening (srv);
    close (srv->sck);
    srv = NULL;
}

void WSWaitForThreadsEnd (webserver_srv_t* srv) {
    pthread_join(srv->listening_thrd, NULL);
}