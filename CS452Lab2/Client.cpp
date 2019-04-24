#include "Client.h"

Client::Client(int port){
    this->port = port;
    this->server = "thing0.cs.uwec.edu";
    //Call the beginning function to start connections and sieving//
    doTheNeedful();
}

void Client::findPrime(){
    for(int i = (this->currPrime); i<= this->max; i++){
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
    int sockfd = callServer(this->port);
    //cout << "Called the Server" << endl;
    recvSize(sockfd);
    do{
        readCurrPrime(sockfd);
            if(max>100000){
                int totalBytes = 0;
                bool recvDone = 0;
                do{
                    int currBytes;
                    if((currBytes = recv(sockfd, bits+totalBytes, bytes, 0)) < 0){
                        error("RECV ERROR IN LOOP ON CLIENT");
                    }
                    if(currBytes == 0){
                        error("CLIENT: SERVER CLOSED CONNECTION");
                    }else{
                        totalBytes += currBytes;
                        if(totalBytes == this->bytes){
                            recvDone = 1;
                        }
                    }
                }while(!recvDone);
            }else{
                recv(sockfd, bits, bytes, 0);
            }
        findPrime();
        //cout << "FOUND PRIME IS " << this->currPrime << endl;
        sieveComposite();
        this->currPrime +=1;
        
        //send current prime back//
        sendCurrPrime(sockfd);
        
        //send the unsigned char* array back//
        send(sockfd, bits, bytes, 0);

    }while(this->currPrime <= sqrt(max));
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
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR CONNECTING TO SERVER");
    }
    return sockfd;
}

void Client::recvSize(int socket){
    int max;
    if(read(socket, &max, sizeof(int)) < 0){
        error("ERROR RECEIVING SIZE TO SOCKET");
    }
    this->bytes = (max/8)+1;
    this->max = max;
    this->bits = (unsigned char*)calloc(this->bytes,1);
    //cout << "RECEIVED SIZE" << endl;
}
void Client::sendCurrPrime(int socket){
    int prime = this->currPrime;
    if(write(socket, &prime, sizeof(int))<0){
        error("ERROR SENDING CURRENT PRIME TO SOCKET");
    }
}

void Client::readCurrPrime(int socket){
    //cout << "READING PRIME FROM SERVER" << endl;
    int prime = 0; 
    if(read(socket, &prime, sizeof(int))<0)
        error("ERROR RECEIVING CURRENT PRIME FROM CLIENT");
    this->currPrime = prime;
}

void Client::error(const char* msg){
    perror(msg);
    exit(0);
}
