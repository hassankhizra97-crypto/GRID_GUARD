#include "../include/GridGuardServer.h"
#include "../include/Simulation.h"
#include <cstring>
#include <fstream>
#include <sstream>

GridGuardServer::GridGuardServer(Simulation& sim, int port)
    : sim_(sim), port_(port), listenSocket_(INVALID_SOCKET), running_(false) {}

GridGuardServer::~GridGuardServer() {
    stop();
}

bool GridGuardServer::initWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << "\n";
        return false;
    }
    return true;
}

bool GridGuardServer::createListenSocket() {
    listenSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket_ == INVALID_SOCKET) {
        std::cerr << "socket() failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return false;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(static_cast<u_short>(port_));

    if (bind(listenSocket_, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
        std::cerr << "bind() failed: " << WSAGetLastError() << "\n";
        closesocket(listenSocket_);
        WSACleanup();
        return false;
    }

    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen() failed: " << WSAGetLastError() << "\n";
        closesocket(listenSocket_);
        WSACleanup();
        return false;
    }

    return true;
}

std::string GridGuardServer::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string GridGuardServer::extractPath(const std::string& request) {
    // Request line: "GET /path HTTP/1.1"
    size_t sp1 = request.find(' ');
    if (sp1 == std::string::npos) return "";
    size_t sp2 = request.find(' ', sp1 + 1);
    if (sp2 == std::string::npos) return "";
    return request.substr(sp1 + 1, sp2 - sp1 - 1);
}

std::string GridGuardServer::extractBody(const std::string& request) {
    size_t headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos) headerEnd = request.find("\n\n");
    if (headerEnd == std::string::npos) return "";
    return request.substr(headerEnd + 4);
}

int GridGuardServer::parseIntParam(const std::string& body, const std::string& key) {
    // Simple JSON key parser: "key":value
    std::string searchKey = "\"" + key + "\"";
    size_t pos = body.find(searchKey);
    if (pos == std::string::npos) return -1;
    size_t colon = body.find(':', pos);
    if (colon == std::string::npos) return -1;
    size_t start = colon + 1;
    while (start < body.size() && (body[start] == ' ' || body[start] == '\t')) start++;
    size_t end = start;
    while (end < body.size() && (isdigit(body[end]) || body[end] == '-' || body[end] == '.')) end++;
    std::string numStr = body.substr(start, end - start);
    try {
        return std::stoi(numStr);
    } catch (...) {
        return -1;
    }
}

std::string GridGuardServer::getMimeType(const std::string& path) {
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".html") return "text/html";
    if (path.size() >= 3 && path.substr(path.size() - 3) == ".js")   return "application/javascript";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".css")  return "text/css";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".svg")  return "image/svg+xml";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".png")  return "image/png";
    if (path.size() >= 4 && path.substr(path.size() - 4) == ".ico")  return "image/x-icon";
    if (path.size() >= 5 && path.substr(path.size() - 5) == ".json") return "application/json";
    return "application/octet-stream";
}

std::string GridGuardServer::makeStaticFileResponse(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        // SPA fallback: serve index.html for any unknown path
        std::ifstream indexFile("frontend/dist/index.html", std::ios::binary);
        if (!indexFile.is_open()) {
            return makeTextResponse(404, "404 Not Found");
        }
        std::ostringstream ss;
        ss << indexFile.rdbuf();
        std::string body = ss.str();
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n";
        oss << "Content-Type: text/html\r\n";
        oss << "Content-Length: " << body.size() << "\r\n";
        oss << "Connection: close\r\n\r\n";
        oss << body;
        return oss.str();
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string body = ss.str();
    std::string mime = getMimeType(filePath);
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: " << mime << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n\r\n";
    oss << body;
    return oss.str();
}


std::string GridGuardServer::makeJsonResponse(int statusCode, const std::string& body) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " OK\r\n";
    oss << "Content-Type: application/json\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    oss << "Access-Control-Allow-Headers: Content-Type\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;
    return oss.str();
}

std::string GridGuardServer::makeTextResponse(int statusCode, const std::string& body) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " OK\r\n";
    oss << "Content-Type: text/plain\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;
    return oss.str();
}

void GridGuardServer::sendResponse(SOCKET clientSocket, const std::string& response) {
    send(clientSocket, response.c_str(), static_cast<int>(response.size()), 0);
    closesocket(clientSocket);
}

std::string GridGuardServer::handleRequest(const std::string& request) {
    std::string method;
    size_t sp = request.find(' ');
    if (sp != std::string::npos) {
        method = request.substr(0, sp);
    }
    method = trim(method);

    std::string path = extractPath(request);

    // Handle CORS preflight
    if (method == "OPTIONS") {
        return makeTextResponse(200, "");
    }

    if (method == "GET") {
        return routeGet(path);
    } else if (method == "POST") {
        std::string body = extractBody(request);
        return routePost(path, body);
    }

    return makeJsonResponse(404, "{\"error\":\"Not Found\"}");
}

std::string GridGuardServer::routeGet(const std::string& path) {
    if (path == "/api/health") {
        return makeJsonResponse(200, "{\"status\":\"ok\",\"service\":\"GridGuard C++ Backend\"}");
    }
    if (path == "/api/network-state") {
        std::string json = sim_.exportNetworkStateJSON();
        return makeJsonResponse(200, json);
    }

    // ── Static file serving (production mode) ──────────────────────────────
    // Serve built React app from frontend/dist when the backend is used
    // as a full-stack server (after `npm run build`).
    std::string distRoot = "frontend/dist";
    // Sanitize path to prevent directory traversal
    std::string safePath = path;
    // Remove query string
    size_t q = safePath.find('?');
    if (q != std::string::npos) safePath = safePath.substr(0, q);
    // Prevent path traversal
    if (safePath.find("..") != std::string::npos) {
        return makeTextResponse(403, "Forbidden");
    }
    // Root → index.html
    if (safePath == "/" || safePath.empty()) safePath = "/index.html";
    std::string filePath = distRoot + safePath;
    return makeStaticFileResponse(filePath);
}

std::string GridGuardServer::routePost(const std::string& path, const std::string& body) {
    if (path == "/api/simulate-failure") {
        int lineId = parseIntParam(body, "lineId");
        int severity = parseIntParam(body, "severity");
        int duration = parseIntParam(body, "durationMinutes");
        if (lineId < 1) {
            return makeJsonResponse(400, "{\"error\":\"lineId required\"}");
        }
        if (severity < 1) severity = 3;
        if (duration < 1) duration = 60;
        bool ok = sim_.simulatePowerLineFailure(lineId, severity, duration);
        std::ostringstream oss;
        oss << "{\"success\":" << (ok ? "true" : "false") << ",\"lineId\":" << lineId << "}";
        return makeJsonResponse(200, oss.str());
    }
    if (path == "/api/process-repair") {
        int faultId = parseIntParam(body, "faultId");
        bool ok;
        if (faultId > 0) {
            ok = sim_.processNextRepair(faultId);
        } else {
            ok = sim_.processNextRepair(-1);
        }
        std::ostringstream oss;
        oss << "{\"success\":" << (ok ? "true" : "false") << "}";
        return makeJsonResponse(200, oss.str());
    }
    if (path == "/api/undo") {
        bool ok = sim_.undoLastOperation();
        std::ostringstream oss;
        oss << "{\"success\":" << (ok ? "true" : "false") << "}";
        return makeJsonResponse(200, oss.str());
    }
    return makeJsonResponse(404, "{\"error\":\"Endpoint not found\"}");
}

bool GridGuardServer::start() {
    if (!initWinsock()) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }
    if (!createListenSocket()) {
        std::cerr << "createListenSocket failed\n";
        return false;
    }
    running_ = true;
    std::cout << "[GridGuard Server] Listening on port " << port_ << std::endl;
    return true;
}

void GridGuardServer::stop() {
    running_ = false;
    if (listenSocket_ != INVALID_SOCKET) {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }
    WSACleanup();
}

void GridGuardServer::run() {
    while (running_) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket_, reinterpret_cast<SOCKADDR*>(&clientAddr), &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            if (!running_) break;
            std::cerr << "accept() failed: " << WSAGetLastError() << "\n";
            continue;
        }

        char buffer[65536];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string request(buffer);
            std::string response = handleRequest(request);
            sendResponse(clientSocket, response);
        } else {
            closesocket(clientSocket);
        }
    }
}