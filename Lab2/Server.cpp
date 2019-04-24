#include "Server.h"

Server::Server(const char* port, const char **argv){
    this->port = port;
    this->max = atoi(argv[2]);
    this->bytes = (atoi(argv[2])/8)+1;
    this->bits = (unsigned char*)calloc(bytes, 1);
    
    //Call the beginning function to start connections and sieving//
    doTheNeedful();
}

void Server::setNeededToOne(){
    for(int i = 2; i <=this->max; i++){
        this->bits[i/8] ^= 1UL << i%8;
    }
}

void Server::findPrime(){
    for(int i = this->currPrime+1; i<= this->max; i++){
        if((int) CHECK_BIT(this->bits[i/8], i%8)){
                this->currPrime = i;
                return; 
        }
    }
}

void Server::sieveComposite(){
    for(int j = (2*this->currPrime); j<= this->max; j+= this->currPrime){
        if((int) CHECK_BIT(this->bits[j/8], j%8))
                this->bits[j/8] ^= 1UL << j%8;
    }
}

void Server::doTheNeedful(){
    setNeededToOne();
    int sockfd = beginListener(atoi(this->port));
    int socket = acceptClient(sockfd);
    sendSize(socket);
    do{
        findPrime();
        sieveComposite();

        //send size of needed unsigned char* array//
        sendCurrPrime(socket);

        send(socket, bits, bytes, 0);
        readCurrPrime(socket);
        recv(socket, bits, bytes, 0);
    }while(currPrime < sqrt(max));
    setPrimes();
    free(bits);

}

int Server::beginListener(int port){
    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd< 0){
            error("ERROR OPENING SOCKET");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR ON BINDING!");
    }

    listen(sockfd,5);
    return sockfd;
}

int Server::acceptClient(int serverSocket){
    int newsockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    cout << "Waiting for a call..." << endl;
    newsockfd = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0){
        error("ERROR ON ACCEPTING CONNECTION!");
    }
    cout << "Connected to client" << endl;
    return newsockfd;
}

void Server::sendSize(int socket){
    int max = this->max;
    if(write(socket, &max, sizeof(int)) < 0){
        error("ERROR SENDING SIZE TO SOCKET");
    }
}
void Server::sendCurrPrime(int socket){
    int currPrime = this->currPrime;
    if(write(socket, &currPrime, sizeof(int))<0){
        error("ERROR SENDING CURRENT PRIME TO SOCKET");
    }
}

void Server::readCurrPrime(int socket){
    int currPrime;
    if(read(socket, &currPrime, sizeof(int))<0);
        error("ERROR GETTING CURRENT PRIME FROM CLIENT");
    this->currPrime = currPrime;
}

void Server::setPrimes(){
    for(int i= 0; i <=(this->max); i++){
        if((int) CHECK_BIT(this->bits[i/8], i%8))
            this->primes.push_back(i);
    }
}

void Server::error(const char* msg){
    perror(msg);
    exit(1);
}



