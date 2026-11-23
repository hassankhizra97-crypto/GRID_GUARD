#ifndef GRIDGUARD_SERVER_H
#define GRIDGUARD_SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#pragma comment(lib, "ws2_32.lib")

class Simulation; // Forward declaration

class GridGuardServer {
public:
    GridGuardServer(Simulation& sim, int port = 8081);
    ~GridGuardServer();

    bool start();
    void stop();
    void run(); // Blocking accept loop

private:
    Simulation& sim_;
    int port_;
    SOCKET listenSocket_;
    bool running_;

    bool initWinsock();
    bool createListenSocket();
    std::string handleRequest(const std::string& request);
    std::string routeGet(const std::string& path);
    std::string routePost(const std::string& path, const std::string& body);
    std::string extractPath(const std::string& request);
    std::string extractBody(const std::string& request);
    int parseIntParam(const std::string& body, const std::string& key);
    std::string makeJsonResponse(int statusCode, const std::string& body);
    std::string makeTextResponse(int statusCode, const std::string& body);
    std::string makeStaticFileResponse(const std::string& filePath);
    void sendResponse(SOCKET clientSocket, const std::string& response);
    std::string trim(const std::string& s);
    std::string getMimeType(const std::string& path);
};

#endif // GRIDGUARD_SERVER_H