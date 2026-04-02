# chat-tcp

A terminal-based chat server and client written in C.

This project is intentionally designed as a learning-focused systems project. The goal is not just to “make chat work”, but to build something that forces me to deal with real systems programming concerns such as:

- multiple clients connecting at the same time
- shared mutable state
- synchronization with mutexes
- blocking socket I/O
- thread lifecycle management
- race conditions
- clean disconnect handling

---

## Why this project?

`chat-tcp` is a compact but realistic systems project. It is small enough to build alone, but rich enough to expose the kinds of bugs and design tradeoffs that appear in real networked software.

It is a good project for practicing:

- socket programming
- concurrent server design
- thread safety
- shared state management
- cleanup and error handling
- debugging difficult timing-dependent bugs

---

## Features

### Core

- TCP server in C
- Terminal-based client in C
- Multiple clients can connect at once
- One thread per connected client
- Broadcast messages to all connected users
- Usernames
- Join / leave notifications
- Graceful disconnect handling

### Maybe extensions idk

These are future ideas, not strict promises:

- Chat rooms
- Private messages
- Server-side command parsing
- Logging
- Thread-safe client registry
- Message queues
- Thread pool version
- `select()` / `poll()` / `epoll()` version for comparison

---

## What this project teaches

### 1. Concurrency

Multiple clients are active at the same time, and the server must handle overlapping events correctly.

Examples:

- two users sending messages at once
- one user disconnecting while another broadcasts
- multiple users joining simultaneously

### 2. Thread Safety

The server maintains shared state such as:

- connected clients
- usernames
- room membership
- message buffers or queues

Because this state is shared across threads, access must be synchronized correctly.

### 3. Multithreading

The initial implementation uses:

- one main thread to accept new connections
- one worker thread per connected client

This model is simple enough to understand but still exposes real threading issues.

### 4. Sockets and Servers

This project uses the standard BSD sockets API, including:

- `socket()`
- `bind()`
- `listen()`
- `accept()`
- `connect()`
- `recv()`
- `send()`
- `close()`

---

## High-level architecture

### Server

The server:

1. creates a listening TCP socket
2. binds to an address and port
3. listens for incoming connections
4. accepts each new client
5. spawns a dedicated thread for that client
6. receives messages from clients
7. broadcasts messages to other connected clients
8. removes clients cleanly on disconnect

### Client

The client:

1. connects to the server
2. sends a username
3. reads user input from the terminal
4. sends messages to the server
5. prints messages received from the server

---
