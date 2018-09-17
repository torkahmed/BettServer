*********************
BETserver Implementation
*********************

A. PRODUCT DESCRIPTION:
-----------------------
A betting game server is accepting clients on tcp sockets, letting the
clients pick a number within a published range, and every 15 seconds
randomize a winning number, and then letting the clients know if they
have won or not.


B. DESIGN OVERVIEW:
-------------------
In this project 2 independant components are included.
1. BETserver (Including the Implementation for BETserver, BETserver_db, and BETstd)
2. BETclient (Including the corresponding BETclient)

They Both use the implemented Library SOCKETwrapper

BETserver.c: Server Implemetation
	> Creating & Listening on a Socket
	> Handling client threads
	> Handling instancing (start/end 15 sec interval)
BETserver_db.c: Handles the database for BETserver module.
	> Saves information about connected clients (ID, Betting Number, etc.)
	> Randomly selects winner when called by BETserver instancing thread
BETclient: Client Implementation
	> Handles communication with server as per the requested protocol


C. KNOWN ISSUES & PENDING TASKS:
--------------------------------

- Message Header sends an extra byte.
	> This could be due to the fact that the message buffer is a struct which can be interpreted differently by different compilers. 
	  Usually it is safer to serialize the data in a uint_8 array and send/receive it that way.

- pthread function pointer takes only 1 argument
	> Workaround Implemented: Since we needed to pass both the client ID and the client socket to the client thread function, a bitwise operation was implemented to concatenate both u32 variables into one u64 and then decode it inside the client thread func.

- Message Send/Recv Protection in BETclient
	> As implemented in BETserver, the BETclient needs to check if it did not receive the complete data, then disconnect.


D. TRACE ABBREVIATIONS: 
-----------------------

[E] Error
[I] Informational Trace
[D] Debug Trace
[U] User Interaction Required