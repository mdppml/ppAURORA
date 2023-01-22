////
//// Created by Mete Akgun on 04.07.20.
////
//
//#ifndef PML_CONNECTION_H
//#define PML_CONNECTION_H
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <chrono>
//
//#include <arpa/inet.h>
//#include <unistd.h> //close
//#include <arpa/inet.h> //close
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
//#include <errno.h>
//#include <netinet/tcp.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <stdint.h>
//#include <iostream>
//#include <cstring>
//#include <fcntl.h>
//#include <iomanip>
//using namespace std;
//
//uint64_t bytesSend = 0;
//uint64_t bytesReceived =0;
//double receive_time = 0.0;
//double send_time = 0.0;
//
//// variables to analyze the runtime of MULv3
//double t0 = 0, t1 = 0, t15 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
//
///*void SetSocketQuickAck(int sock)
//{
//     int value = 1;
//     int res = setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, &value, sizeof(value));
//
//}*/
//
//void SetSocketNonBlock(int sock){
//    int res = fcntl(sock, F_SETFL, O_NONBLOCK);
//}
//void Receive(int socket, uint8_t* buffer, size_t sz){
//    auto start = chrono::high_resolution_clock::now();
//    size_t psz = 65536;
////    size_t psz = 8096;
//    size_t tsz = 0;
//    //cout<<sz<<" bytes will be received from "<<socket<<endl;
//    while (tsz < sz){
//        if (sz-tsz <= psz)
//            psz = sz-tsz;
//
//        int received = recv(socket , buffer , psz , 0);
//        if (received >= 0){
//            tsz+=received;
//            buffer+=received;
//        }
//    }
//    bytesReceived+=sz;
//    auto end = chrono::high_resolution_clock::now();
//    double time_taken =
//            chrono::duration_cast<chrono::nanoseconds>(end - start).count();
//    time_taken *= 1e-9;
//    receive_time += time_taken;
//}
//void Send(int socket, uint8_t* buffer, size_t sz){
//    auto start = chrono::high_resolution_clock::now();
//    size_t psz = 65536;
////    size_t psz = 8096;
//    size_t tsz = 0;
//    //cout<<sz<<" bytes will be sent to "<<socket<<endl;
//    while (tsz < sz){
//        if (sz-tsz <= psz)
//            psz = sz-tsz;
//
//        int sent = send(socket , buffer , psz , 0);
//        if (sent >= 0){
//            tsz+=sent;
//            buffer+=sent;
//        }
//
//    }
//    bytesSend+=sz;
//    auto end = chrono::high_resolution_clock::now();
//    double time_taken =
//            chrono::duration_cast<chrono::nanoseconds>(end - start).count();
//    time_taken *= 1e-9;
//    send_time += time_taken;
//}
//
//void SetBufferSizes(int socket){
//    int sendbuff;
//    socklen_t optlen;
//    int res = 0;
//    // Get buffer size
//    optlen = sizeof(sendbuff);
//    res = getsockopt(socket, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);
//
//    if(res == -1)
//        printf("Error getsockopt one");
//    else
//        printf("send buffer size = %d\n", sendbuff);
//
//    // Set buffer size
//    sendbuff = 131072;
//
//    printf("sets the send buffer to %d\n", sendbuff);
//    res = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));
//
//    int recvbuff;
//    res = 0;
//    // Get buffer size
//    optlen = sizeof(recvbuff);
//    res = getsockopt(socket, SOL_SOCKET, SO_RCVBUF, &recvbuff, &optlen);
//
//    if(res == -1)
//        printf("Error getsockopt one");
//    else
//        printf("receive buffer size = %d\n", recvbuff);
//
//    // Set buffer size
//    recvbuff = 131072;
//
//    printf("sets the recv buffer to %d\n", recvbuff);
//    res = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &recvbuff, sizeof(recvbuff));
//}
//
//void open_helper(string haddress, uint16_t hport1, uint16_t hport2, int *client_socket) {
//    int server_fd1, new_socket1, server_fd2, new_socket2;
//    struct sockaddr_in address;
//
//    int addrlen = sizeof(address);
//    // Creating socket file descriptor
//    if ((server_fd1 = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//
//    // Forcefully attaching socket to the port 8080
//    int opt = 1;
//    if (setsockopt(server_fd1, SOL_SOCKET, SO_REUSEADDR,
//                   &opt, sizeof(opt)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = inet_addr(haddress.c_str());
//    address.sin_port = htons( hport1 );
//    // Forcefully attaching socket to the port 8080
//    if (::bind(server_fd1, (struct sockaddr *)&address,
//               sizeof(address))<0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd1, 3) < 0)
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//    if ((new_socket1 = accept(server_fd1, (struct sockaddr *)&address,
//                             (socklen_t*)&addrlen))<0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//    int flag = 1;
//    if (setsockopt(new_socket1,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//    //SetSocketNonBlock(new_socket1);
//    SetBufferSizes(new_socket1);
//
//    uint8_t buffer[1];
//    Receive(new_socket1,buffer,1);
//    if (buffer[0] == 1)
//        printf("P1 is connected\n");
//    else{
//        printf("P1 did not connect to helper\n");
//        exit(-1);
//    }
//
//    buffer[0] = 0;
//    Send(new_socket1 , buffer , 1);
//    client_socket[0] = new_socket1;
//
//
//    // Creating socket file descriptor
//    if ((server_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//
//    // Forcefully attaching socket to the port 8080
//    opt = 1;
//    if (setsockopt(server_fd2, SOL_SOCKET, SO_REUSEADDR,
//                   &opt, sizeof(opt)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = inet_addr(haddress.c_str());
//    address.sin_port = htons( hport2 );
//    // Forcefully attaching socket to the port 8080
//    if (::bind(server_fd2, (struct sockaddr *)&address,
//               sizeof(address))<0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd2, 3) < 0)
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//    if ((new_socket2 = accept(server_fd2, (struct sockaddr *)&address,
//                             (socklen_t*)&addrlen))<0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//    flag = 1;
//    if (setsockopt(new_socket2,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//    //SetSocketNonBlock(new_socket2);
//    SetBufferSizes(new_socket2);
//
//    Receive( new_socket2 , buffer, 1);
//    if (buffer[0] == 2)
//        printf("P2 is connected\n");
//    else{
//        printf("P2 did not connect to helper\n");
//        exit(-1);
//    }
//
//    buffer[0] = 0;
//    Send(new_socket2 , buffer , 1);
//    client_socket[1] = new_socket2;
//}
//
//
//int connect2helper(const string cip,const string hip, uint16_t cport, uint16_t hport, int r) {
//    int sock = 0;
//    while (1) {
//        struct sockaddr_in serv_addr;
//        if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
//            printf("\n Socket creation error \n");
//            return -1;
//        }
//
//        serv_addr.sin_family = AF_INET;
//        serv_addr.sin_port = htons(hport);
//
//        // Convert IPv4 and IPv6 addresses from text to binary form
//        if (inet_pton(AF_INET, hip.c_str(), &serv_addr.sin_addr) <= 0) {
//            printf("\nInvalid address/ Address not supported \n");
//            return -1;
//        }
//
//        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
//            sock=0;
//            sleep(1);
//        } else {
//           break;
//        }
//        int flag =1;
//        if (setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
//        {
//            perror("setsockopt");
//            exit(EXIT_FAILURE);
//        }
//        //SetSocketNonBlock(sock);
//
//        SetBufferSizes(sock);
//
//    }
//    uint8_t buffer[1];
//    if (r==0)
//        buffer[0] = 1;
//    else
//        buffer[0] = 2;
//    Send(sock, buffer, 1);
//    Receive( sock , buffer, 1);
//    if (buffer[0] == 0)
//        printf("Helper is connected\n");
//    else{
//        printf("Helper did not connect\n");
//        exit(-1);
//    }
//    return sock;
//}
//int open_P1(string caddress, uint16_t cport){
//    int server_fd, new_socket;
//    struct sockaddr_in address;
//    int opt = 1;
//    int addrlen = sizeof(address);
//    // Creating socket file descriptor
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//
//    // Forcefully attaching socket to the port 8080
//    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
//                   &opt, sizeof(opt)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = inet_addr(caddress.c_str());
//    address.sin_port = htons( cport );
//    // Forcefully attaching socket to the port 8080
//    if (::bind(server_fd, (struct sockaddr *)&address,
//             sizeof(address))<0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//    if (listen(server_fd, 3) < 0)
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
//                             (socklen_t*)&addrlen))<0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//    int flag = 1;
//    if (setsockopt(new_socket,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//    //SetSocketNonBlock(new_socket);
//    SetBufferSizes(new_socket);
//    uint8_t buffer[1];
//    Receive( new_socket , buffer, 1);
//    if (buffer[0] == 2)
//        printf("P2 is connected\n");
//    else{
//        printf("P2 did not connect to P1\n");
//        exit(-1);
//    }
//    buffer[0] = 1;
//    Send(new_socket , buffer ,1);
//    return new_socket;
//}
//int connect2P1(const string cip, uint16_t cport){
//    int sock = 0;
//    while (1) {
//        struct sockaddr_in serv_addr;
//        if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
//            printf("\n Socket creation error \n");
//            return -1;
//        }
//
//
//        serv_addr.sin_family = AF_INET;
//        serv_addr.sin_port = htons(cport);
//
//        // Convert IPv4 and IPv6 addresses from text to binary form
//        if (inet_pton(AF_INET, cip.c_str(), &serv_addr.sin_addr) <= 0) {
//            printf("\nInvalid address/ Address not supported \n");
//            return -1;
//        }
//
//        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
//            sock=0;
//            sleep(1);
//        } else{
//            break;
//        }
//        int flag = 1;
//        if (setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
//        {
//            perror("setsockopt");
//            exit(EXIT_FAILURE);
//        }
//        //SetSocketNonBlock(sock);
//        SetBufferSizes(sock);
//
//
//
//    }
//    uint8_t buffer[1];
//    buffer[0] = 2;
//    Send(sock , buffer , 1);
//    Receive( sock , buffer, 1);
//    if (buffer[0] == 1)
//        printf("P1 is connected\n");
//    else{
//        printf("P1 did not connect to P2\n");
//        exit(-1);
//    }
//    return sock;
//}
//
//void PBytes(){
//    cout<<"Bytes Sent:\t"<<bytesSend<<endl;
//    cout<<"Bytes Received:\t"<<bytesReceived<<endl;
//    cout<<"Send Time\t" << fixed << send_time << setprecision(9) << " sec" << endl;
//    cout<<"Receive Time\t" << fixed << receive_time << setprecision(9)<< " sec" << endl;
//
////    cout << "------------------------" << endl;
////    cout << "t0: " << fixed << t0 << setprecision(9) << " sec" << endl;
////    cout << "t1: " << fixed << t1 << setprecision(9) << " sec" << endl;
////    cout << "t15: " << fixed << t15 << setprecision(9) << " sec" << endl;
////    cout << "t2: " << fixed << t2 << setprecision(9) << " sec" << endl;
////    cout << "t3: " << fixed << t3 << setprecision(9) << " sec" << endl;
////    cout << "t4: " << fixed << t4 << setprecision(9) << " sec" << endl;
////    cout << "t5: " << fixed << t5 << setprecision(9) << " sec" << endl;
////    cout << "Total: " << fixed << (t1 + t2 + t3 + t4 + t5) << setprecision(9) << " sec" << endl;
////    cout << "------------------------" << endl;
//}
//void close_socket(int socket){
//    cout<<socket<<endl;
//    close(socket);
//}
//void printRTT(int socket){
//    struct timespec start, finish;
//    clock_gettime( CLOCK_REALTIME, &start );
//    size_t result = send(socket , (uint8_t*)"Hello" , 5 , 0);
//    if (result < 0) {
//        printf("Error while sending data");
//        exit(EXIT_FAILURE);
//    }
//    uint8_t buffer[5];
//    result = recv(socket , buffer , 5 , 0);
//    if (result < 0) {
//        printf("Error while sending data");
//        exit(EXIT_FAILURE);
//    }
//    clock_gettime( CLOCK_REALTIME, &finish );
//    printf( "%f\n", ((double) (finish.tv_nsec - start.tv_nsec))/((double) 100000) );
//}
//
//#endif //PML_CONNECTION_H


// ================= NEW CONNECTION FILE ===============================


//
// Created by Mete Akgun on 04.07.20.
//

#ifndef PML_CONNECTION_H
#define PML_CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <chrono>

#include <arpa/inet.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <errno.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <iomanip>
#include <thread>
using namespace std;

uint64_t bytesSend = 0;
uint64_t bytesReceived =0;
double receive_time = 0.0;
double send_time = 0.0;


void RTimers(){
    receive_time = 0.0;
    send_time = 0.0;
}
int openscoket(string address, uint16_t port){
    int server_fd, sock;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address.c_str());
    addr.sin_port = htons(port);
    if (::bind(server_fd, (struct sockaddr *)&addr, sizeof(addr))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((sock = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen))<0){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    int flag =1;
    if (setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    close(server_fd);
    return sock;
}
int connect2socket(string address, uint16_t port){
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("\n Socket creation error \n");
        exit(-1);
    }

    int flag =1;
    if (setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char *)&flag,sizeof(int)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(-1);
    }

    while (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        sleep(1);
    }
    return sock;
}
void Rcv(int socket,uint8_t* buffer, size_t sz){
    size_t left = sz;
    while (left > 0){
        int received = read(socket , &(buffer)[sz-left] , left);
        left -= received;
    }
}
void Snd(int socket,uint8_t* buffer, size_t sz){
    size_t left = sz;
    while (left > 0){
        int sent = write(socket ,  &(buffer)[sz - left]  , left);
        left -= sent;
    }
}
void Receive(int *socket, uint8_t* buffer, size_t sz){
    thread thr[SCKNUM];
    int block_size = (int)ceil(sz*1.0/SCKNUM);
    if (block_size == 0)
        block_size = sz;
    for (int i=0;i<SCKNUM;i++){
        thr[i] = thread(Rcv,*(socket+i),buffer+(i*block_size),block_size);
    }
    for (int i=0;i<SCKNUM;i++){
        thr[i].join();
    }
    bytesReceived+=sz;
}
void Send(int *socket, uint8_t* buffer, size_t sz){
    thread thr[SCKNUM];
    int block_size = (int)ceil(sz*1.0/SCKNUM);
    if (block_size == 0)
        block_size = sz;
    for (int i=0;i<SCKNUM;i++){
        thr[i] = thread(Snd,*(socket+i),buffer+(i*block_size),block_size);
    }
    for (int i=0;i<SCKNUM;i++){
        thr[i].join();
    }
    bytesSend+=sz;
}

void open_helper(string haddress, uint16_t hport, int *socket_p0, int *socket_p1) {
    for (int i=0;i<2*SCKNUM;i++){
        int new_socket = openscoket(haddress,hport+i);
        uint8_t buffer[1];
        int rd = 0;
        while (rd != 1)
            rd = read(new_socket,buffer,1);
        if (buffer[0] == 2)
            printf("P%d is connected on port %d \n", i/SCKNUM, hport+i);
        else{
            printf("P%d did not connect to helper on port %d \n", i/SCKNUM, hport+i);
            exit(-1);
        }

        buffer[0] = 2;
        rd = 0;
        while (rd != 1)
            rd = write(new_socket , buffer , 1);
        if (i<SCKNUM)
            socket_p0[i] = new_socket;
        else
            socket_p1[i-SCKNUM] = new_socket;
    }
}


void connect2helper(const string address, uint16_t port, int *socket_helper) {
    for (int i=0;i<SCKNUM;i++){
        socket_helper[i] = connect2socket(address, port+i);
        uint8_t buffer[1];
        buffer[0] = 2;
        int rd = 0;
        while (rd != 1)
            rd = write(socket_helper[i], buffer, 1);
        rd = 0;
        while (rd != 1)
            rd = read( socket_helper[i] , buffer, 1);
        if (buffer[0] == 2)
            printf("Helper is connected on port %d \n", port+i);
        else{
            printf("Helper did not connect on port %d\n", port+i);
            exit(-1);
        }
    }
}

void open_P0(string address, uint16_t port, int *socket_p1){
    for (int i=0;i<SCKNUM;i++){
        socket_p1[i] = openscoket(address, port+i);
        uint8_t buffer[1];
        int rd = 0;
        while (rd != 1)
            rd = read(socket_p1[i] , buffer, 1);
        if (buffer[0] == 1)
            printf("P1 is connected on port %d\n", port+i);
        else{
            printf("P1 did not connect to P0 on port %d\n", port+i);
            exit(-1);
        }
        buffer[0] = 0;
        rd = 0;
        while (rd != 1)
            rd = write(socket_p1[i] , buffer ,1);
    }
}
void connect2P0(const string address, uint16_t port, int *socket_p0){
    for (int i=0;i<SCKNUM;i++){
        socket_p0[i] = connect2socket(address, port+i);
        uint8_t buffer[1];
        buffer[0] = 1;
        int rd = 0;
        while (rd != 1)
            rd = write(socket_p0[i] , buffer ,1);
        rd = 0;
        while (rd != 1)
            rd = read(socket_p0[i] , buffer, 1);
        if (buffer[0] == 0)
            printf("P0 is connected on port %d\n", port+i);
        else{
            printf("P0 did not connect to P1 on port %d\n", port+i);
            exit(-1);
        }
    }
}
void closesocket(int *sock){
    for (int i=0;i<SCKNUM;i++){
        close(sock[i]);
    }
}

void PBytes(){
    cout<<"Bytes Sent:\t"<<bytesSend<<endl;
    cout<<"Bytes Received:\t"<<bytesReceived<<endl;
    cout<<"Receive Time\t"<<fixed
        << receive_time << setprecision(9) << " sec" << endl;
    cout<<"Send Time\t"<<fixed
        << send_time << setprecision(9) << " sec" << endl;
}

#endif //PML_CONNECTION_H

