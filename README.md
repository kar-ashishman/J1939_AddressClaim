# Simulation of Address Claim Protocol J1939 

A C implementation simulating **SAE J1939 Address Claim Porotocol** over a UDP broadcast network. This is a full multi threaded address claim simulation.
A real CAN bus broadcasts frames to all nodes simultaneously, this simulation replicates that behavious using UDP broadcast over LAN

## Background 
In a network, before a node can communicate, it must claim a unique **Source Address** - 1 BYTE identifier (0x00 - 0xFD) that identifies the node on the bus.
The address claim protocol works as follows:
1. A node broadcasts an **Address Claim Message** (PGN `EE00`) containing its 8 byte NAME and Desired SA
2. If another node already holds that SA and has a lower NAME value ( higher priority) , it defends the claim and the node requesting for the SA must reclaim with another address.
3. If no conflict is detected within **250ms**, the address is considered successfully claimed.
4. A node that cannot claim any address sends a **Cannot Claim Address** message (SA = `0xFE`)
## Project structure 
### Architecture
---
```
address_claim_library.c     — Main J1939 logic (TX thread, RX thread, conflict detection)
address_claim_proto.h       — Function prototypes for the address claim application
node.c                      — UDP socket layer (init, send, recv, cleanup)
node_public.h               — node_t struct definition
node_defs.h                 — Constants: PORT=50000, BUFFLEN=20
node_proto.h                — Function prototypes for node layer
Makefile                    — Build system
```
### Abstracted UDP Layer
---
Created a abstrcated UDP layer, whose APIs are called to send and receive datagram packets by the Address Claim Application software.
Three core functions that make up the UDP transport layer implementation.
##### **`node_init() :`** Runs once at startup , and initialises sockets and threads in requence
- `socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)` Asks the kernel to allocat a UDP sockets.
- `setsockopt(SO_BROADCAST)` Initialises send to broadcast addresses by default.
- `setsockopt(SO_REUSEPORT)` Enable reuse address for the socket
- `bind(INADDR_ANY,PORT 5000)` Registers the socket with the OS port table. `INADDR_ANY` means accept packets on any network interface.
After this, every UDP datagram arriving on port `5000` - including broadcast frames from every other node on the subnet, is routed into this sockets receive buffer.
This replicates the CAN bus behaviour.
- `pthread_mutex_init` and `pthread_cond_init()` Initialises the mutex and conditional variables.
They help in protecting shared states between the RX and TX thread  
##### **`node_send()`** : This is a fucntin call, to transmit data. It takes whatever is already in node->send_buffer and fires it as a single UDP datagram to the subnet broadcast addresses
- `sendto()` is used because UDP is connectionless - There is no established connection to send "into". The destination address must be provided on every call.
- Because destination is a broadcast address, every node on the subnet with a socket bound to port `5000` will receive this datagram - Including the sender itself.
This is apt J1939 behavious , where a node monitors its own frame.
##### **`node_recv()`** : The transport layer fills the node->rcv_buffer.
- recvfrom(node->sock, node->rcv_buffer, BUFFER_LENGTH, 0, (struct sockaddr*)&from, &fromlen) blocks the thread until the UDP datagram arrives. UDP sends discrete datagrams.
- The received bytes are stored into the `node->rcv_buffer` Each recvfrom()  on the receiver always returns exactly one datagram.
- Never overlapped / split. If the buffer is too small for the datagram, the excess bytes are discarded - They are never splilt/buffered for the next call. 
### Address Claim Application 
---
Multiple instances of the application running in separate terminals simulates multiple nodes contending for addresses. Each node consists of a unique source address that is assigned to it. 
##### Threading Model 
```
main()
├── RX Thread (rx_func)  — runs forever, blocks on recvfrom(), detects conflicts
└── TX Thread (tx_func)  — sends Address Claim, waits 250ms, re-claims on conflict
```
Both threads share a single `node_t` struct and one UDP socket. Thread synchronization uses `pthread_mutex_t` + `pthread_cond_t`. 
##### Address Claim Message Frame Layout
```
Byte 0: 0x18          — Priority field
Byte 1: 0xEE          — PGN high byte (Address Claimed = 0xEE00)
Byte 2: node->sa      — Source Address
Byte 3: 0xFF          — Destination (global broadcas)
Bytes 4–11: NAME      — 8-byte J1939 NAME (not yet populated in current impl)
```
#### Design
##### State Machine
---
```
                    ┌─────────────┐
    Power On ──────►│    INIT     │
                    └──────┬──────┘
                           │ Send Address Claim
                           ▼
                    ┌─────────────┐◄─────────────────┐
                    │  CLAIMING   │                  │
                    │  (stat=1)   │                  │
                    └──────┬──────┘                  │
                           │ Wait 250ms              │
                    ┌──────┴──────┐                  │
                    │  Conflict?  │                  │
                    └──┬──────┬───┘                  │
                  YES  │      │ NO                   │
                       │      ▼                      │
              ┌────────┴─┐  ┌─────────────┐          │
              │ Compare  │  │   CLAIMED   │          │
              │  NAMEs   │  │   (stat=2)  │          │
              └──┬────┬──┘  └──────┬──────┘          │
            WIN  │    │ LOSE       │ New conflict    │
                 │    │            └─────────────────┘
         Re-assert    │ Pick new SA                  |
         my claim     └──────────────────────────────►
                        (loop back to CLAIMING)
```
#### Thread Interaction
---
```
TX Thread                              RX Thread
─────────────────────────────────      ───────────────────────────────────
send Address Claim                     recvfrom() ← BLOCKING
lock mutex                             
cond_timedwait(250ms) ◄────────────── packet arrives       → conflict_check()
  │                                        │
  │  ETIMEDOUT (250ms, no conflict)        ├─ own packet   → continue (ignore)
  └► stat=2, ADDRESS CLAIMED               │
                                           ├─ we WIN       → re-send claim directly
  rc=0 (woken early by RX)                 │
  └► pick new SA, re-send ◄──────────────  └─ we LOSE      → lock + cond_signal
```
##### Conflict Resolution
---
```
received NAME < my NAME  →  They WIN  →  I pick new SA, re-send claim
received NAME > my NAME  →  I WIN     →  Re-broadcast my claim immediately
received NAME == my NAME →  Own packet (loopback) → Ignore
```
## Build & Run
---
### Prerequisites
- MinGW with `gcc` and `mingw32-make`
- `pthreads` library (included with MinGW)
### Build
```bash
cd AddressClaimSimulation/
mingw32-make app
```
### Run (Two Nodes — Conflict Simulation)
Open two terminals in `AddressClaimSimulation/`:
```bash
# Terminal 1
.\app.exe
# Enter name: 100
# Enter address: 1

# Terminal 2
.\app.exe
# Enter name: 200
# Enter address: 1    ← same address triggers conflict!
# Node with NAME=100 wins (lower NAME = higher priority)
# Node with NAME=200 picks a new address
```
### Clean
```bash
mingw32-make clean
```
