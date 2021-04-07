#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h> // socket, bind
#include <sys/socket.h> // socket, bind, listen, inet_ntoa
#include <netinet/in.h> // htonl, htons, inet_ntoa
#include <arpa/inet.h> // inet_ntoa
#include <netdb.h> // gethostbyname
#include <unistd.h> // read, write, close
#include <string.h> // bzero
#include <netinet/tcp.h> // SO_REUSEADDR
#include <sys/uio.h> // writev

using namespace std;
//Program must take in six arguments:
// serverPort - server's port number
// serverName - server's IP address or host name
// iterations - number of iterations
// nbufs	  - number of data buffers
// bufsize	  - the size of each data buffer (bytes)
// type		  - the type of transfer

int main(int argc, char* argv[]) {
	//first, create a "help" flag for users to get info about the program
	if (argc < 3) {
		if (string(argv[1]) == "--help" || string(argv[1]) == "-h") {
			cout << "This is the help function. We'll write something else"
			<< "here soon\n";
			return 0;
		}
	}

	//Check that there are enough parameters
	// If there are too many parameters, the extra ones will not be considered
	if (argc < 7) {
		cerr << "ERROR: not enough arguments";
		return 1;
	}
	
	//Define the parameters
	char* serverPort = argv[1];
	char* serverName = argv[2];
	int iterations = atoi(argv[3]);
	int nbufs = atoi(argv[4]);
	int bufsize = atoi(argv[5]);
	int type = atoi(argv[6]);

	//Check that the type is valid
	if (type < 1 || type > 3) {
		cerr << "ERROR: invalid type entered. Must be type 1, 2, or 3";
		return 1;
	}

	//Declare addrinfo and servinfo
	struct addrinfo hints;
	struct addrinfo *servInfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(serverName, serverPort, &hints, &servInfo);

	//1. Open a new socket and create a secure connection
	//Open
	int clientSd = socket(servInfo->ai_family, servInfo->ai_socktype, 
	servInfo->ai_protocol);
	//Connect
	int status = connect(clientSd, servInfo->ai_addr, servInfo->ai_addrlen);
	if(status < 0){
		cerr << "Failed to connect to the server\n";
		close(clientSd);
		return -1; 
	}


	//2. allocate the data buffers
	char** databuf = new char* [nbufs];
	for (int i = 0; i < nbufs; i++){
		databuf[i] = new char[bufsize];			//Where nbufs * bufsize = 1500
	}

	for (int i = 0; i < nbufs; i++) {
		for (int j = 0; j < bufsize; j++) {
			databuf[i][j] = 'A';
		}
	}

	//3. Start a timer by calling gettimeofday
	struct timeval startTime;
	int start = gettimeofday(&startTime, NULL);
	struct timeval lapTime;
	
	//4. Repeat the iteration times of data transfers 
	//Perform action needed for given type
	switch (type) {
	case 1:
		//Multiple Writes
		for (int i = 0; i < iterations; i++) {
	 		for (int j = 0; j < nbufs; j++)
	 			write(clientSd, databuf[j], bufsize);
	 	}
	 	break;
	case 2:
		for(int i = 0; i < iterations; i++){
			struct iovec vector[nbufs];
			for(int j = 0; j < nbufs; j++){
				vector[j].iov_base = databuf[j];
				vector[j].iov_len = bufsize;
			}
			writev(clientSd, vector, nbufs);
		}
		break;
	case 3:
		for(int i = 0; i < iterations; i++){
			write(clientSd, (void*)databuf, nbufs * bufsize);
		}
		break;
	default:
		cerr << "It is not possible to be here\n";
		return 1;
	}

	//5. Lap the timer by calling gettimeofday
	// where lap - start = data-transmission time
	int lap = gettimeofday(&lapTime, NULL);
	struct timeval data_transmission;
	timersub(&lapTime, &startTime, &data_transmission);

	//6. Receive from the server an acknowledgement that shows how many times
	// the server called read( ).
	int count = 0;
	int ret = read(clientSd, &count, sizeof(count));


	//7. Stop the timer by calling gettimeofday,
	// where stop - start = round-trip time.
	struct timeval stopTime;
	struct timeval roundtrip;
	timersub(&stopTime, &startTime, &roundtrip);

	cout << "Test 1: data-transmission time = " << data_transmission.tv_usec
	<< " usec, round-trip time = " << roundtrip.tv_usec  << " usec, #reads = "
	<< count << endl;

	//deallocate all instances of new
	for (int i = 0; i < nbufs; i++)
		delete[] databuf[i];
	delete[] databuf;
	
	close(clientSd);
	return 0;
}