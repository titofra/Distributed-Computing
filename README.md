# Distributed Computing

[Distributed computing](https://en.wikipedia.org/wiki/Distributed_computing) refers to a network of computers working together to solve a problem or complete a task. Instead of a single computer handling all the processing, the workload is divided among multiple machines, allowing for faster and more efficient processing.

## Scope
Here is a basic implementation of a generic distributed computing: you can give any task to any kind of network. Indeed, clients and server can be almost anything, including computers, servers, etc ... even phones ??? There is no limits (I hope). Finally, network can be heterogeneous which means you can have differents kinds of system, maybe on running on various architectures, with various specificities.

## Implementation
### Introduction
Firstly, a computer named "sever" know all the tasks and his role is to spread it over the workers. The workers, also named clients, are computers that are waiting for a task. There unique role is to execute tasks.

### Connections
Server and clients are connected through bi-directional sockets. Moreover, a webserver is running on the server.

### How it works
Once the server get a new task, it checks if there is a non-working clients. If there is, it send it the task's id through socket. The workers, which is continuously waiting for a task, now get one. However it may need some data to run the task. For that, it fetch the webpage associated to the task. Finally, it run the task, send the result to the server through the socket and waits for another task.

## Run
- Start the webserver with Node.js ```$node ./webserver.js```
- Then execute the server ```$./server.o```
- Now the server is ready to establish connections, you just need to setup some clients. For that, run the os-image on the clients' machine.

### Build the clients' os-image
To build the ISO file for the clients, go to /os and run ```make os-image``` and follow intructions. Note that you will have to download a Linux kernel and busybox.

## Examples
A template program can be found in /examples/template/ which provide a minimal implementation.
Other examples are coming soon.

## Warnings
This very basic distributed computing program is unstable. I am still working on. There is a lot of issues, it is not efficient, it is not user-friendly, etc ... but it should works. I am open to any advice/helps/contributes, so fill free to help the project !

## TODO
[] To test. Do I really need to setup different local addr ip for each client? I guess yes
[] Delete client and don't crash on socket's client interruption
[] Change my dirty system of mutex
[] Stop workers if needed
[] Show stats on index file
[] Memory leak
[] Don't fix maximum legnth for the number of char of task & the lenght of client return data
[] Use the same connection/socket between client and webserver (Connection keep-alive)
[] 
