# Simple-Database-Client-Server
This project demonstrates a simple client-server application to manage a basic database. Clients can PUT or GET database records based on a unique ID.
**Features:**
------------------------------------------------------------
- **Client Application** (`dbclient.c`): Allows users to interact with the database server. Supports PUT to add entries and GET to retrieve entries.
- **Server Application** (`dbserver.c`): Listens for client requests and manages the database file `database.txt`.

**Compilation and Execution:**
--------------------------
Make sure you have `gcc` installed. The application uses the POSIX threads library, so ensure your environment supports it.

Compile:
--------
```bash
make

**Run Server:**
./dbserver [PORT]

Run Client:

./dbclient [HOSTNAME] [PORT]

**Usage:**
For the Client: Once you run the client, you'll be prompted with a menu:

Enter your choice (1 to put, 2 to get, 0 to quit):
Choose 1 to PUT a record: You'll need to provide a name and a unique ID.
Choose 2 to GET a record: You'll need to provide an ID to retrieve the corresponding record.
Choose 0 to quit the client.
For the Server:

The server runs indefinitely listening for client requests. It will automatically handle PUT and GET operations.

Database:
The database is a simple file (database.txt) managed by the server. Each record contains a name (string) and an ID (unsigned integer).

**Code Structure:**
dbclient.c: Contains the client's main application logic.
dbserver.c: Contains the server's main application logic. Uses threads to handle multiple clients concurrently.
msg.h: Header file containing the message structure used for communication between the client and server.
Makefile.txt: Makefile for compiling the client and server applications.
Known Limitations:
The server handles client requests using separate threads. However, concurrent writes to the database.txt file by multiple threads might lead to race conditions. For a production-grade application, consider using mutex locks or a more robust database solution.
