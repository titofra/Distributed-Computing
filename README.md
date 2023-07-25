# Distributed Computing

[Distributed computing](https://en.wikipedia.org/wiki/Distributed_computing) refers to a network of computers working together to solve a problem or complete a task. Instead of a single computer handling all the processing, the workload is divided among multiple machines, allowing for faster and more efficient processing.

<p align="center">
	<img src="https://github.com/titofra/Distributed-Computing/raw/main/resources/distributed_computing.png" width="500">
</p>

<br/>
⚠️ This repository contains submodules, add --recurse-submodules when cloning
<br/>

## Scope
This is a basic implementation of a generic distributed computing system that can be used to distribute any task across a **network of computers**. The clients and servers can be any type of computer, including desktops, servers, and even phones. There are no limitations (I hope) to the types of systems that can be used, and the network can be heterogeneous, meaning that different systems with different architectures and specifications can be used together.

## Implementation
### Introduction
The server is responsible for managing the tasks and distributing them to the workers. The workers, also known as clients, are computers that are waiting for tasks to be assigned to them. There unique role is to execute tasks.

### Connections
Server and clients are connected through bidirectional sockets. Moreover, a web server is running on the server.

### How it works
When the server receives a new task, it checks to see if there are any available workers. If there is at least one, it sends the task ID to one worker through the socket. The worker receives the task and may need additional data to complete it. To obtain this data, it fetches the webpage associated with the task. Once it has all the necessary data, it runs the task, sends the results back to the server through the socket, and waits for another task.

## Running the system
- Start your server (check the examples below for more information)
- The server is now ready to establish connections, and clients can be set up by running the os-image on their machines.

### Build the lib
To build the lib, just run the appropriate script.

### Build the clients' os-image
To build the ISO file for the clients, navigate to the /os and run ```make os-image```, following the instructions provided. Note that you will need to download a Linux kernel and busybox.

## Examples
A template program can be found in [/examples/template/](https://github.com/titofra/Distributed-Computing/tree/main/examples/template), which provides a minimal implementation. Other examples are forthcoming.

## Warnings
This very basic distributed computing program is **unstable**. I am still working on. There are a lot of issues: it is not efficient, it is not user-friendly, etc ... but it should works. I am open to any advice/helps/contributions, so fill free to help the project !

## TODO
- [ ] To test. Do I really need to set up different local IP addresses for each client? I guess yes
- [ ] Implement a "delete client" function and don't crash on socket's client interruption
- [ ] Change my dirty system of mutex
- [ ] Implement the ability to stop workers if necessary
- [ ] Display statistics on the index file
- [ ] Memory leak
- [ ] Remove the maximum length limit for task character length and client return data
- [ ] Use the same connection/socket between the client and webserver (Connection keep-alive)
