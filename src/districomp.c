#include "districomp.h"

void InitServer (districomp_srv_t* srv, const uint16_t port, const uint16_t ws_port, const int domain, const int type, const int protocol, const char* path_to_ws_root, const uint16_t maxSockReqQeued) {
    // Create socket
    srv->mainSock = socket (domain, type, protocol);
    if (srv->mainSock < 0) {
        perror ("\nCan't create socket\n");
        exit (1);
    }

    // Forcefully attaching socket to the port
    int opt = 1;
    if (setsockopt(
        srv->mainSock,
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
    if (bind (srv->mainSock, (struct sockaddr*)&srv->addr, sizeof(srv->addr)) < 0){
        perror ("\nCouldn't bind the socket to the port\n");
        exit (1);
    }

    // Init
    srv->path_to_ws_tsk = (char*) malloc (strlen (path_to_ws_root) + 6 +1);
    strcpy (srv->path_to_ws_tsk, path_to_ws_root);
    strcpy (srv->path_to_ws_tsk + strlen (path_to_ws_root), "tasks/\0");
    srv->nb_cli = 0;
    srv->nb_tsk = 0;
    srv->clients = (_client_t*) malloc (0);
    srv->tasks = (_task_t*) malloc (0);
    InitMailBox (&srv->waitingTasks, 128, -1, false);   // TODO unlimited mailbox (not 128)
    InitMutex (&srv->clients_mut);
    InitMutex (&srv->tasks_mut);
    InitWebServer (&srv->webSrv, ws_port, domain, type, protocol, path_to_ws_root);
    StartWSListening (&srv->webSrv, maxSockReqQeued);

    // TODO: build path if non existing
}

void StartListening (districomp_srv_t* srv, const uint16_t maxSockReqQeued) {
    // Listen for clients
    if (listen (srv->mainSock, maxSockReqQeued) < 0){
        perror ("\nCan't listen the socket\n");
        exit (1);
    }

    // Create the listening thread
    pthread_create(&srv->listening_thrd, NULL, &_Listening, srv);
}

void* _Listening (void* arg) {
    districomp_srv_t* srv = (districomp_srv_t*) arg;

    int tsk_id;
    while (1) {

        // Should we cancel the thread?
        pthread_testcancel ();

        // Accept an incoming connection
        struct sockaddr_in client_addr;
        socklen_t client_size = sizeof(client_addr);
        srv->tmp_cli_sck = accept(srv->mainSock, (struct sockaddr*)&client_addr, &client_size);

        if (srv->tmp_cli_sck > 0){

            AcquireMutex (&srv->clients_mut, true);

            // Add the client to clients
            srv->clients = (_client_t*) realloc (srv->clients, (srv->nb_cli + 1) * sizeof (_client_t));
            srv->clients [srv->nb_cli].addr = client_addr;
            srv->clients [srv->nb_cli].sck = srv->tmp_cli_sck;
            srv->clients [srv->nb_cli].tsk_id = -1;
            srv->nb_cli += 1;

            // There is no need of Set access for srv->clients
            ReleaseMutex (&srv->clients_mut, true);
            AcquireMutex (&srv->clients_mut, false);
            printf("\nNew client connected: IP %s:%i\n",
                inet_ntoa(srv->clients [srv->nb_cli - 1].addr.sin_addr),
                ntohs(srv->clients [srv->nb_cli - 1].addr.sin_port)
            );
            ReleaseMutex (&srv->clients_mut, false);
        
            // As this is a new client, we can assign him a task
            tsk_id = Receive (&srv->waitingTasks);
            if (tsk_id >= 0) {

                // We need Set access for srv->clients
                AcquireMutex (&srv->clients_mut, true);

                // There is a new task for him
                srv->clients [srv->nb_cli - 1].tsk_id = tsk_id;

                ReleaseMutex (&srv->clients_mut, true);
                AcquireMutex (&srv->clients_mut, false);

                _SendTskCli (srv, srv->nb_cli - 1);

                ReleaseMutex (&srv->clients_mut, false);
            }
        } else {
            printf ("\nCan't accept the client at IP %s:%i\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port)
            );
        }
    }
}

void StopListening (districomp_srv_t* srv) {
    close (srv->tmp_cli_sck);   // close the temporary socket
    pthread_cancel (srv->listening_thrd);
}

void StartReceiving (districomp_srv_t* srv) {
    // Create receiving thread
    pthread_create (&srv->receiving_thrd, NULL, &_Receiving, srv);
}

void* _Receiving (void* arg) {
    districomp_srv_t* srv = (districomp_srv_t*) arg;
    int tsk_id;
    uint16_t nb_cli;
    while (1) {

        // Should we cancel the thread?
        pthread_testcancel ();

        AcquireMutex (&srv->clients_mut, false);
        nb_cli = srv->nb_cli;
        ReleaseMutex (&srv->clients_mut, false);

        // For each client
        char buf [128];
        uint16_t i = 0;
        while (i < nb_cli) {

            AcquireMutex (&srv->clients_mut, false);

            // If is working
            if (srv->clients [i].tsk_id >= 0) {

                AcquireMutex (&srv->tasks_mut, true);

                // We may receive a message
                ssize_t nb = 0;
                bool recv_something = false;
                while ((nb = recv (
                        srv->clients [i].sck,
                        buf,
                        sizeof(buf),
                        MSG_DONTWAIT    // Don't wait until a msg is receive
                    )) >= 0
                ){
                    recv_something = true;

                    size_t previous_len = strlen (srv->tasks [srv->clients [i].tsk_id].result);
                    srv->tasks [srv->clients [i].tsk_id].result = (char *) realloc (
                        srv->tasks [srv->clients [i].tsk_id].result, 
                        sizeof (srv->tasks [srv->clients [i].tsk_id].result) + sizeof (char) * (unsigned long int) (nb + 1)
                    );
                    strcpy (srv->tasks [srv->clients [i].tsk_id].result + previous_len, buf);
                    srv->tasks [srv->clients [i].tsk_id].result [previous_len + (size_t) nb + 1] = 0;   // make sure the string is closed
                }

                if (recv_something) {
                    ReleaseMutex (&srv->clients_mut, false);

                    srv->tasks [srv->clients [i].tsk_id].isDone = true;

                    AcquireMutex (&srv->clients_mut, true);
                    // Reset state
                    srv->clients [i].tsk_id = -1;
                    ReleaseMutex (&srv->clients_mut, true);

                    // As the client is release we can assign him a new task
                    tsk_id = Receive (&srv->waitingTasks);
                    if (tsk_id >= 0) {
                        // There is a new task for him

                        AcquireMutex (&srv->clients_mut, true);

                        srv->clients [i].tsk_id = tsk_id;

                        ReleaseMutex (&srv->clients_mut, true);
                        AcquireMutex (&srv->clients_mut, false);

                        _SendTskCli (srv, i);

                        ReleaseMutex (&srv->clients_mut, false);
                    }
                } else {
                    ReleaseMutex (&srv->clients_mut, false);

                    // It may be not a failed, just there is no msg
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        // This is a real msg that failed
                        perror ("\nCouldn't receive data from client\n");
                        exit (1);
                    }
                }

                ReleaseMutex (&srv->tasks_mut, true);

            } else {
                ReleaseMutex (&srv->clients_mut, false);
            }

            i += 1;

            // We ask for an access just after released in the case there is a set access waiting (yes i know it s dirty)
            AcquireMutex (&srv->clients_mut, false);
            nb_cli = srv->nb_cli;
            ReleaseMutex (&srv->clients_mut, false);
        }

        // Sleep 1s was the only solution i found to get a valid srv->nb_cli (not constant)
        //sleep (1);
    }
}

void StopReceiving (districomp_srv_t* srv) {
    pthread_cancel (srv->receiving_thrd);
}

int AddTask (districomp_srv_t* srv, const char* data) {

    AcquireMutex (&srv->tasks_mut, true);

    // Add the task to tasks
    srv->tasks = (_task_t*) realloc (srv->tasks, (srv->nb_tsk + 1) * sizeof (_task_t));
    srv->tasks [srv->nb_tsk].id = srv->nb_tsk;
    srv->tasks [srv->nb_tsk].data_in = data;
    srv->tasks [srv->nb_tsk].result = (char*) malloc (1);
    srv->tasks [srv->nb_tsk].result [0] = 0;    // init result to a string of one character... the end character
    srv->tasks [srv->nb_tsk].isDone = false;
    srv->nb_tsk += 1;

    ReleaseMutex (&srv->tasks_mut, true);
    AcquireMutex (&srv->tasks_mut, false);

    _AddDataWSFileTsk (srv, srv->nb_tsk - 1);

    ReleaseMutex (&srv->tasks_mut, false);
    AcquireMutex (&srv->clients_mut, true);

    // Search a non-working clients
    uint16_t i = 0;
    while (i < srv->nb_cli && srv->clients [i].tsk_id >= 0) {
        i ++;
    }

    AcquireMutex (&srv->tasks_mut, false);

    if (i < srv->nb_cli) {
        // Found one
        srv->clients [i].tsk_id = srv->nb_tsk - 1;
        _SendTskCli (srv, i);
    } else {
        // We can't, we add it to the mailbox
        Send (&srv->waitingTasks, srv->nb_tsk - 1);
    }

    uint16_t tsk_id = srv->nb_tsk - 1;

    ReleaseMutex (&srv->tasks_mut, false);
    ReleaseMutex (&srv->clients_mut, true);

    // Return the task id
    return tsk_id;
}

// should be run at least with a get acces for srv->tasks_mut
void _AddDataWSFileTsk (districomp_srv_t* srv, uint16_t tsk_id) {
    // Build path
    char path [strlen (srv->path_to_ws_tsk) + N_CHR_TSK_MAX];
    char tsk_id_str [N_CHR_TSK_MAX];
    if (snprintf (tsk_id_str, N_CHR_TSK_MAX, "%d", tsk_id) >= N_CHR_TSK_MAX) {
        printf ("\nN_CHR_TSK_MAX not enough large\n");
        exit (1);
    }
    strcat (strcpy (path, srv->path_to_ws_tsk), tsk_id_str);

    // Create the file
    FILE* pfile = fopen (path, "w");
    if (pfile != NULL) {
        // Write the data into the file
        fwrite (srv->tasks [tsk_id].data_in, sizeof (char), strlen (srv->tasks [tsk_id].data_in), pfile);
    } else {
        perror ("\nCannot create/access file for task. Make sure you provided a path to a valid folder containing at least a folder named 'tasks/'\n");
        exit (1);
    }

    fclose (pfile);
}

// Should be executed with at least srv->clients Get access
void _SendTskCli (districomp_srv_t* srv, uint16_t cli_id) {
    char msg [N_CHR_TSK_MAX];
    int n = snprintf (msg, N_CHR_TSK_MAX, "%d", srv->clients [cli_id].tsk_id);
    if (n >= N_CHR_TSK_MAX) {
        printf ("\nN_CHR_TSK_MAX not enough large\n");
        exit (1);
    }

    // Replace the rest of the characters to a chracter other than a number
    // (cause '\0' cannot be send by socket?)
    for (int i = n; i < N_CHR_TSK_MAX; i++) {
        msg [i] = '_';
    }

    if (send (srv->clients [cli_id].sck, msg, strlen(msg), 0) < 0){
        perror ("\nCan't send message to give task to client\n");
        exit (1);
    }
}

bool AreSleeping (districomp_srv_t* srv) {

    AcquireMutex (&srv->clients_mut, false);

    for (uint16_t i = 0; i < srv->nb_cli; i++) {
        if (srv->clients [i].tsk_id >= 0) {
            // At least one client is working

            ReleaseMutex (&srv->clients_mut, false);

            return false;
        }
    }

    ReleaseMutex (&srv->clients_mut, false);

    return true;
}

char* GetResult (districomp_srv_t* srv, int task_id) {
    return srv->tasks [task_id].result;
}

void ClearTasks (districomp_srv_t* srv) {
    // Clear tasks' array
    srv->nb_tsk = 0;

    // Clear waiting tasks
    while (Receive (&srv->waitingTasks) >= 0) {
        // while there is task...
    }

    // TODO: stop workers
}

uint16_t GetNbClients (districomp_srv_t* srv) {

    AcquireMutex (&srv->clients_mut, false);
    uint16_t nb_cli = srv->nb_cli;
    ReleaseMutex (&srv->clients_mut, false);

    return nb_cli;
}

uint16_t GetNbTasks (districomp_srv_t* srv) {
    
    AcquireMutex (&srv->tasks_mut, false);
    uint16_t nb_tsk = srv->nb_tsk;
    ReleaseMutex (&srv->tasks_mut, false);

    return nb_tsk;
}

uint16_t GetNbRemainingTasks (districomp_srv_t* srv) {
    uint16_t nb_tsk = 0;
    
    AcquireMutex (&srv->tasks_mut, false);

    for (uint16_t i = 0; i < srv->nb_tsk; i++) {
        if (srv->tasks [i].isDone == false) {
            nb_tsk ++;
        }
    }
    ReleaseMutex (&srv->tasks_mut, false);

    return nb_tsk;
}

void CloseServer (districomp_srv_t* srv) {
    StopListening (srv);
    StopReceiving (srv);
    StopWSListening (&srv->webSrv);
    ClearTasks (srv);
    DestroyMutex (&srv->clients_mut);
    DestroyMutex (&srv->tasks_mut);
    for (int i = 0; i < srv->nb_cli; i++) {
        close (srv->clients [i].sck);
    }
    close (srv->mainSock);
    CloseWebServer (&srv->webSrv);
    free (srv->tasks);
    free (srv->clients);
    srv = NULL;
}

void WaitForThreadsEnd (districomp_srv_t* srv) {
    // This makes the main thread wait on the death of the threads
    pthread_join(srv->listening_thrd, NULL);
    pthread_join(srv->receiving_thrd, NULL);
    WSWaitForThreadsEnd (&srv->webSrv);
}

void InitClient (districomp_cli_t* cli, const char* addr, const uint16_t sock_port, const int domain, const int type, const int protocol, const uint16_t ws_port) {
    // Create socket
    cli->srv_sck = socket (domain, type, protocol);
    if (cli->srv_sck < 0) {
        perror ("\nCan't create server socket\n");
        exit (1);
    }

    cli->domain = domain;
    cli->type = type;
    cli->protocol = protocol;

    // Set port/ip
    cli->srv_addr.sin_family = (sa_family_t) domain;    // may do some conversion error
    cli->srv_addr.sin_port = htons(sock_port);
    cli->ws_addr.sin_family = (sa_family_t) domain;    // may do some conversion error
    cli->ws_addr.sin_port = htons(ws_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton((sa_family_t) domain, addr, &cli->srv_addr.sin_addr) <= 0) {
        perror ("\nInvalid address, Address not supported/recognised\n");
        exit (1);
    }
    if (inet_pton((sa_family_t) domain, addr, &cli->ws_addr.sin_addr) <= 0) {
        perror ("\nInvalid address, Address not supported/recognised\n");
        exit (1);
    }

    // Connect
    if (connect(cli->srv_sck, (struct sockaddr*)&cli->srv_addr, sizeof(cli->srv_addr)) < 0) {
        perror ("\nConnection to sever Failed\n");
        exit (1);
    }
}

void ReceiveTask (districomp_cli_t* cli, char** result) {
    // Read the task id from socket
    char tsk_id [N_CHR_TSK_MAX];
    while (recv (cli->srv_sck, tsk_id, N_CHR_TSK_MAX, 0) <= 0); // TODO cause it is dirty

    // Add the '\0' character at the end to close the string
    uint16_t i = 0;
    while (i < N_CHR_TSK_MAX && (int) tsk_id [i] < 58 && (int) tsk_id [i] > 47) {
        // while the charcter is a number
        i++;
    }
    if (i < N_CHR_TSK_MAX) {
        tsk_id [i] = 0;
    } else {
        printf ("\nError related to N_CHR_TSK_MAX: it may be to tiny for the task's id\n");
        exit (1);
    }

    // Connect to webserver
    cli->ws_sck = socket (cli->domain, cli->type, cli->protocol);
    if (cli->ws_sck < 0) {
        perror ("\nCan't create webserver socket\n");
        exit (1);
    }
    if (connect(cli->ws_sck, (struct sockaddr*)&cli->ws_addr, sizeof(cli->ws_addr)) < 0) {
        perror ("\nConnection to webserver Failed (Have you lanched the webserver?)\n");
        exit (1);
    }

    // Load data from the webserver
    const uint16_t webreq_size_max = 130 + N_CHR_TSK_MAX;   // TODO cause it is dirty
    char webreq [webreq_size_max];
    if (snprintf (
        webreq,
        webreq_size_max,
        "GET /tasks/%s HTTP/1.0\r\nHost: %s:%i\r\nContent-type: text/html; charset=utf-8\r\nContent-length: 0\r\n\r\n",
        tsk_id,
        inet_ntoa (cli->ws_addr.sin_addr),
        ntohs (cli->ws_addr.sin_port)
        ) < 0)
    {
        perror ("\nError while loading webpage's request\n");
        exit (1);
    }

    // write the request
    if (send (cli->ws_sck, webreq, strlen (webreq), 0) >= 0) {
        // Read the response
        ssize_t n;
        const uint16_t buf_size = 512;
        uint16_t nb_buf_size = 0;
        char* data = (char*) malloc (nb_buf_size * buf_size * sizeof (char));
        char buf [buf_size];
        while ((n = recv(cli->ws_sck, buf, buf_size, 0)) > 0) {
            buf [n] = 0;    // close the string

            // Add the buffer to data
            nb_buf_size ++;
            data = (char*) realloc (data, nb_buf_size * buf_size * sizeof (char));
            strcat (data, buf);
        }

        // Remove the header
        while (
            strlen (data) > 3
            && (
                data [0] != '\r'
                || data [1] != '\n'
                || data [2] != '\r'
                || data [3] != '\n'
            )
        ) {
            data += 1;    // header not found
        }

        if (strlen (data) <= 3) {
            printf ("\nHeader not found !\n");
            exit (1);
        }
        data += 4;    // pass the CRLF CRLF aka "\r\n\r\n" string that delimites the end of the header

        *result = data;

    } else {
        perror ("\nSend to webserver failed\n");
        exit (1);
    }

    // Close the connection
    close (cli->ws_sck);
}

void SendResult (districomp_cli_t* cli, const char* data) {
    send (cli->srv_sck, data, strlen(data), 0);
}

void CloseClient (districomp_cli_t* cli) {
    close (cli->srv_sck);
    cli = NULL;
}