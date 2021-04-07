#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/uio.h>
#include <pthread.h>
#include <sys/time.h>

using namespace std;
// Program takes in two arguments:
//  port - the server's port number
//  iterations - the number of iterations of client's data

const int BUFSIZE = 1500;
const int MY_ID = 8499;

struct thread_data{
    int repetitions = 0;
    int sd = 0;
};

void* serverThreadFunc(void* data);

int main(int argc, char* argv[]) {
	//first, create a "help" flag for users to get info about the program
	if (argc < 3) {
		if (string(argv[1]) == "--help" || string(argv[1]) == "-h") {
			cout << "This is the help function. We'll write something else"
            << " here soon\n";
			return 0;
		}
        //return if there were not enough parameters
        cerr << "ERROR: not enough arguments";
		return 1;
	}

    //Define the parameters
    int port = atoi(argv[1]);
    int iterations = atoi(argv[2]);

 

    //Declare addrinfo structure
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    char buf[sizeof(int)*3+2];
    snprintf(buf, sizeof buf, "%d", port);
    getaddrinfo(NULL, buf, &hints, &res);

    //Open stream-oriented socket 
    int serverSd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // Set SO_REUSEADDR option
    const int yes = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

    //Bind socket to its local address by calling bind
    bind(serverSd, res->ai_addr, res->ai_addrlen);

    //Instruct OS to listen to up to n connection requests
    listen(serverSd,10);

    //Receive a request from a client by calling accept
    struct sockaddr_storage newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    //int newSd = accept(serverSd, (struct sockaddr*)&newSockAddr, &newSockAddrSize);
    while (1){
        int newSd = accept(serverSd, (struct sockaddr *)&newSockAddr,
         &newSockAddrSize);
        pthread_t newThread;
        thread_data *data = new thread_data;
        data->repetitions = iterations;
        data->sd = newSd;
        int iret1 = pthread_create(&newThread, NULL, &serverThreadFunc, (void*)data);
        pthread_join(newThread, NULL);
        //cout << iret1;
        


        close(newSd);
    
    }

    return 0;
}


void* serverThreadFunc(void* data){
    //1. Allocate databuf[BUFSIZE], where BUFSIZE = 1500.
    char* databuf = new char[BUFSIZE];
    thread_data* data2 = (thread_data*)data;

    //2. Start a timer by calling gettimeofday.
    struct timeval startTime;
    int start = gettimeofday(&startTime, NULL);

    //3. Repeat reading data from the client into databuf[BUFSIZE].
    int count = 0;
    for(int i = 0; i < data2->repetitions; i++){
        for(int nRead = 0; nRead < BUFSIZE; ++count) {
            int ret = read(data2->sd, databuf, BUFSIZE - nRead);
            nRead += ret;
        }
        //count++;
    }
    

    //4. Stop the timer by calling gettimeofday, where stop - start = data-receiving time.
    struct timeval stopTime;
    int stop = gettimeofday(&stopTime, NULL);
    struct timeval data_receiving;
    timersub(&stopTime, &startTime, &data_receiving);
    cout << "Test 1: data-receiving time " << data_receiving.tv_usec << " usec\n";
    //5. Send the number of read( ) calls made, (i.e., count in the above) as an
    //acknowledgement.
    write(data2->sd, &count, sizeof(count));
    //6. Print out the statistics as shown below: 

    delete [] databuf;
    close(data2->sd);
    return NULL;
}