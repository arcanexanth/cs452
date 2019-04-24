#include "Client.h"

Client::Client(const char* port){
    this->port = port;
 
    //Call the beginning function to start connections and sieving//
    doTheNeedful();
}

void Client::findPrime(){
    for(int i = this->currPrime+1; i<= this->max; i++){
        if((int) CHECK_BIT(this->bits[i/8], i%8)){
                this->currPrime = i;
                return; 
        }
    }
}

void Client::sieveComposite(){
    for(int j = (2*this->currPrime); j<= this->max; j+= this->currPrime){
        if((int) CHECK_BIT(this->bits[j/8], j%8))
                this->bits[j/8] ^= 1UL << j%8;
    }
}

void Client::doTheNeedful(){
    int sockfd = callServer(atoi(this->port));
    cout << "Called the Server" << endl;
    recvSize(sockfd);
    do{
        readCurrPrime(sockfd);
        recv(sockfd, bits, bytes, 0);
        findPrime();
        sieveComposite();
        
        //send current prime back//
        sendCurrPrime(sockfd);
        
        //send the unsigned char* array back//
        send(sockfd, bits, bytes, 0);

    }while(currPrime < sqrt(max));
    free(bits);

}

int Client::callServer(int port){
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        error("ERROR CALLING SERVER");
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    struct hostent *server;
    if((server = gethostbyname(this->server))==NULL){
        error("HOST SERVER NOT FOUND!");
    }
    return sockfd;
}

void Client::recvSize(int socket){
    int max;
    if(read(socket, &max, sizeof(int)) < 0){
        error("ERROR RECEIVING SIZE TO SOCKET");
    }
    this->bytes = (max/8)+1;
    this->bits = (unsigned char*)calloc(this->bytes,1);
}
void Client::sendCurrPrime(int socket){
    int prime = this->currPrime;
    if(write(socket, &prime, sizeof(int))<0){
        error("ERROR SENDING CURRENT PRIME TO SOCKET");
    }
}

void Client::readCurrPrime(int socket){
    int prime;
    if(read(socket, &prime, sizeof(int))<0);
        error("ERROR RECEIVING CURRENT PRIME FROM CLIENT");
    this->currPrime = prime;
}

void Client::error(const char* msg){
    perror(msg);
    exit(1);
}
