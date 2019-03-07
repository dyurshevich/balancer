#include <iostream>
#include <sstream>
#include <signal.h>
#include "proxyserver.h"

using namespace std;

ProxyServer server;

vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   istringstream tokenStream(s);
   while (getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

void printHelp()
{
    cout << "Usage: balancer upstreams port" << endl;
    cout << "Example: balancer '192.168.0.1,192.168.0.2' 80" << endl;
    cout << "Note: execute with root privileges if use port number less than 1024" << endl;
}

void sig_exit(int s)
{
    cout << "Exit signal!" << endl;
    server.stop();
}

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        printHelp();
        return 0;
    }
    signal(SIGINT, sig_exit);

    vector<string> upstreams = split(argv[1], ',');
    if (upstreams.size() == 0) {
        cout << "Error: no upstreams" << endl;
        return -1;
    }

    int port = 0;
    try {
        port = stoi(argv[2]);
    } catch (const std::invalid_argument &e) {
        cout << "Error: invalid port number" << endl;
        return -1;
    }

    server.setPort(port);
    server.setUpstreams(upstreams);
    server.start();

    return 0;
}
