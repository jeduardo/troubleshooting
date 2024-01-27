# network payload tester

The goal of this code is to send packets of well-defined size to a remote server
with content that is familiar to the human eye to assist in the analysis of
packet fragmentation over network tunnels.

## Components

The `server` side listens for a stream of data to be saved in a buffer 3 times
the size of the amount of bytes we want to have in the payload. This should
allow us to see the data stream being fragmented by the underlying TCP/IP
implementation. It continuously listens for new client connections until its
execution is interrupted.

The `client` side connects to a remote server and sends a stream of data that is
3 times the number of bytes specified. The goal is to observe how the underlying
TCP/IP implementation will fragment the data stream to reach the counterpart. It
runs the workload a single time.

## Usage

In the server, run:

```shell
./server <port> <bytes>
```

In the client, run:

```shell
./client <server address> <port> <bytes>
