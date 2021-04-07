Client Server Program
Rosemary Rosvanis 2021
CSS 432

The files included in this program are the Client.cpp file and the Server.cpp program
which will run on two different computers

To compile the Server.cpp file use the command "g++ Server.cpp -o Server -lpthread". 
To compile the Client.cpp file use the command "g++ Client.cpp -o Client".

The Server program needs to be ran before the Client program.
The Server program takes two arguments - the port number and iterations.
For this program, I recommend using 8499 as the port number, as this is the last 4
digits of my student ID. For iterations, this program has been testing with up to
20000 iterations.

Once the Server program is running, you can begin to run your Client program.
The Client function takes six arguments: the port number, the server name, iterations,
nbufs, bufsize, and type.
For port number, use the same as used in the Server program: 8499.
For server name use the name of the server host (i.e. "csslabX.uwb.edu" where X is 
a computer number)
For iterations use the same number as was chosen when running server.
nbufs and bufsize must multiply together to make 1500.
The type refers to writes in which 1 = muliple writes, 2 = writev, and 3 = single write
