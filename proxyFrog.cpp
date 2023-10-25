/*
 * Environment: WSL (Windows Subsystem for Linux)
 * Language: C++
 * Additional: Used for HTTP servers (not HTTPS)
 */

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <regex>
#include <netdb.h>

using namespace std;

#define PORT 3700
#define MAXLINE 4096

// Function which parses through the website contents and makes the modifications
string getWebPageContent(const string& url, const string& path) {
    int web_socket = 0;

    // Creating the socket
    web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
   
    struct hostent* address;
    // Searching for the website url
    address = gethostbyname(url.c_str());
    if (address == nullptr) {
        perror("Host not found!");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(80);
    memcpy(&server_address.sin_addr.s_addr, address->h_addr, address->h_length);

    // Connection
    int status = connect(web_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr_in));
    if (status == -1) {
        perror("Connect failed!");
        exit(EXIT_FAILURE);
    }

    // Format an HTTP GET request for the desired website path
    string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + url + "\r\n";
    request += "Connection: close\r\n\r\n";  // Ask the server to close the connection after sending the response

    // // DEBUGGING STATEMENTS
    // cout << request << endl;
    
    // Send the request
    send(web_socket, request.c_str(), request.length(), 0);
    cout << "HTTP request sent" << endl;

    string response;
    char buffer[MAXLINE] = {0};
    int bytesReceived = 0;

    while ((bytesReceived = recv(web_socket, buffer, MAXLINE, 0)) > 0) {
        // Convert the received data to a string
        string data(buffer, bytesReceived);
        response += data;
        memset(buffer, 0, MAXLINE);
    }
    
    // ===================== Replacing frog with Fred using regex =====================
    regex pattern("frog ", regex_constants::icase);
    response = regex_replace(response, pattern, " Fred ");

    // ===================== Replacing jpg urls with a specific image url (frogue) =====================

    // Define markers for image source URLs
    string imgStart = "<img ";
    string imgEnd = ".jpg\"";

    // Initialize the position for searching the start of an image tag
    size_t tagBegin = response.find(imgStart);

    // Loop through the HTML content to find and replace image tags
    while (tagBegin != string::npos) {  // npos == "until the end of the string"
        // Find the position of the end of the current image tag
        size_t tagEnd = response.find(imgEnd, tagBegin + imgStart.length());

        if (tagEnd != string::npos) {        // Check if the end of the current image tag was found
            // Extract the entire image tag from the HTML content
            string imgTag = response.substr(tagBegin, tagEnd - tagBegin + imgEnd.length());
            // Find the position of the "src" attribute within the image tag
            size_t srcPos = imgTag.find("src=\"");
            
            if (srcPos != string::npos) { // Check if "src" attribute was found within the image tag
                // Find the position of the closing double quotation mark of the "src" attribute value

                size_t srcEndPos = imgTag.find("\"", srcPos + 5);
                if (srcEndPos != string::npos) {  // If double quotation mark was found
                    // Replace the "src" attribute value with the desired Frogue URL
                    response.replace(tagBegin + srcPos, srcEndPos - srcPos + 1, "src=\"http://pages.cpsc.ucalgary.ca/~jcleahy/Frogue.jpg\"");
                }
            }
        }
        tagBegin = response.find(imgStart, tagBegin + imgStart.length());  // Find position of the start of the next image tag
    }

    // =====================
    // Calculate the new content length
    string::size_type conPos = response.find("Content-Length:");  // find position
    if (conPos != string::npos) {  // Check if the "Content-Length:" header was found
        string::size_type conEndPos = response.find("\r\n", conPos);  // Find end of header (end of current line)
        if (conEndPos != string::npos) {   // If end found
            // Extract the part of the response before the "Content-Length:" header
            string beforeCon = response.substr(0, conPos);
            // Calculate the new "Content-Length" value by subtracting the length of the removed header
            string newCon = "Content-Length: " + to_string(response.size() - beforeCon.size());
            // Replace the existing "Content-Length" header with the new value
            response.replace(conPos, conEndPos - conPos, newCon);
        }
    }

    // DEBUGGING STATEMENTS
    // cout << response << endl;

    // Close the web socket
    close(web_socket);
    return response;
}

int main() {

    // Initialization of variables
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating the socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "Server started at port " << PORT << ". Waiting for connections..." << endl;

    while (true) {
        // Accepting incoming connections
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        cout << "Socket accepted" << endl;

        // Receive the HTTP request from the client
        char buffer[MAXLINE] = {0};
        int bytesReceived = recv(new_socket, buffer, MAXLINE, 0);
        if (bytesReceived <= 0) {     // Close the socket and continue to the next client
            close(new_socket);
            continue;
        }

        string request(buffer);
        // Use a regular expression to extract the requested path
        regex pathRegex("GET ([^ ]+) HTTP/1.1");
        smatch match;
        if (regex_search(request, match, pathRegex) && match.size() > 1) {
            string path = match[1].str();  // Extract path

            // Fetch the web page content for the requested path
            string webContent = getWebPageContent("pages.cpsc.ucalgary.ca", path);

            // Send the web page content as an HTTP response
            send(new_socket, webContent.c_str(), webContent.size(), 0);
        } else {
            // Respond with a 400 Bad Request if the request is invalid
            string response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nInvalid request";
            send(new_socket, response.c_str(), response.size(), 0);
        }

        close(new_socket);  // Close socket
    }

    return 0;
}
