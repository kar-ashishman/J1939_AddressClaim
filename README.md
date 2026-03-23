# Simulation of Address Claim Protocol J1939 

A C implementation simulating **SAE J1939 Address Claim Porotocol** over a UDP broadcast network. This is a full multi threaded address claim simulation.
A real CAN bus broadcasts frames to all nodes simultaneously, this simulation replicates that behavious using UDP broadcast over LAN
---
## Background 
In a network, before a node can communicate, it must claim a unique **Source Address** - 1 BYTE identifier (0x00 - 0xFD) that identifies the node on the bus.
The address claim protocol works as follows:
1. A node broadcasts an **Address Claim Message** (PGN `EE00`) containing its 8 byte NAME and Desired SA
2. If another node already holds that SA and has a lower NAME value ( higher priority) , it defends the claim and the node requesting for the SA must reclaim with another address.
3. If no conflict is detected within **250ms**, the address is considered successfully claimed.
4. A node that cannot claim any address sends a **Cannot Claim Address** message (SA = `0xFE`)
---
## Project structure 
### Abstracted UDP Layer
Created a abstrcated UDP layer, whose APIs are called to send and receive datagram packets by the Address Claim Application software.
Three core functions that make up the UDP transport layer implementation.
#### **`node_init() :`** Runs once at startup , and initialises sockets and threads in requence
- `socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)` Asks the kernel to allocat a UDP sockets.
- `setsockopt(SO_BROADCAST)` Initialises send to broadcast addresses by default.
- `setsockopt(SO_REUSEPORT)` Enable reuse address for the socket
- `bind(INADDR_ANY,PORT 5000)` Registers the socket with the OS port table. `INADDR_ANY` means accept packets on any network interface.
After this, every UDP datagram arriving on port `5000` - including broadcast frames from every other node on the subnet, is routed into this sockets receive buffer.
This replicates the CAN bus behaviour.
- `pthread_mutex_init` and `pthread_cond_init()` Initialises the mutex and conditional variables.
They help in protecting shared states between the RX and TX thread  
#### **`node_send()`** : This is a fucntin call, to transmit data. It takes whatever is already in node->send_buffer and fires it as a single UDP datagram to the subnet broadcast addresses
- `sendto()` is used because UDP is connectionless - There is no established connection to send "into". The destination address must be provided on every call.
- Because destination is a broadcast address, every node on the subnet with a socket bound to port `5000` will receive this datagram - Including the sender itself.
This is apt J1939 behavious , where a node monitors its own frame.
#### **`node_recv()`** : The transport layer fills the node->rcv_buffer.
- recvfrom(node->sock, node->rcv_buffer, BUFFER_LENGTH, 0, (struct sockaddr*)&from, &fromlen) blocks the thread until the UDP datagram arrives. UDP sends discrete datagrams.
- The received bytes are stored into the `node->rcv_buffer` Each recvfrom()  on the receiver always returns exactly one datagram -
- Never overlapped / split. If the buffer is too small for the datagram, the excess bytes are discarded - They are never splilt/buffered for the next call. 

### Address Claim Application 
#### How the three work together
```
main()
  │
  ├── node_init()             ← one-time: socket(), bind(), mutex, cond variable
  │
  ├── [recv thread] node_recv()
  │      └── recvfrom() blocks ──→ frame arrives ──→ address_claim_parser()
  │                                                          │
  │                                               (protocol layer decides
  │                                                to send a claim frame)
  │                                                          │
  └── [send thread] node->send_hdlr()                       ↓
                         └── node_send() ──→ sendto() ──→ broadcast
                                                            │
                                              all nodes' recvfrom() wake up
   
```   
#### Message Frame Layout (send buffer)

```
Byte 0: 0x18          — Priority field
Byte 1: 0xEE          — PGN high byte (Address Claimed = 0xEE00)
Byte 2: node->sa      — Source Address
Byte 3: 0xFF          — Destination (global broadcas)
Bytes 4–11: NAME      — 8-byte J1939 NAME (not yet populated in current impl)
```
#### Application 

Multiple instances of the application running in separate terminals simulates multiple nodes contending for addresses. Each node consists of a unique source address that is assigned to it. Each instance of the application does the following things on sequence ( during power up):
##### Node Initilisation
The `node_t` struct is a node , holding function pointers , socket handle, receive buffer, transmit buffer, NAME, SA and state of the node.
```
typedef struct node_t{
        //handlers
        int(*init_hdlr)(struct node_t *node);               // Initialize handler
        void(*send_hdlr)(struct node_t *node);               // Send handler 
        void(*recv_hdlr)(struct node_t *node);               // Receive handler
        void(*cleanup_hdlr)(struct node_t *node);           // Cleanup handler
        // socket tied to the node
        int sock;
        // buffers
        unsigned char *rcv_buffer;
        unsigned char *send_buffer;
        // Every node has a 64 bit NAME
        unsigned char name[8];
        // Source address 1 byte long
        unsigned char sa;
        // Address table 
        unsigned char table[256][9];
        // condtion wait for receiving thread
        pthread_mutex_t lock1;
        pthread_cond_t recv_cond1;
        pthread_mutex_t lock2;
        pthread_cond_t recv_cond2;
        // state info
        unsigned char state;
    } node_t;
```
###### Node states
```
UNCLAIMED → CLAIMING → CLAIMED
                ↓ 
           CLAIM_FAILED 
```
##### Sending thread is spawned

##### Receiving thread is spawned

