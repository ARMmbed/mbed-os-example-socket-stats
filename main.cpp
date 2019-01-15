/* Copyright (c) 2018 Arm Limited
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "mbed.h"

#if !defined(MBED_CONF_NSAPI_SOCKET_STATS_ENABLE) 
#error [NOT_SUPPORTED] Socket Statistics not supported
#endif

#define SAMPLE_TIME     10 // milli-sec
#define COMPLETED_FLAG (1UL << 0)

PlatformMutex stdio_mutex;
EventFlags threadFlag;

void print_stats()
{
    mbed_stats_socket_t stats[MBED_CONF_NSAPI_SOCKET_STATS_MAX_COUNT];
    static int num = 0;
    int count;

    memset(&stats[0], 0, sizeof(mbed_stats_socket_t) * MBED_CONF_NSAPI_SOCKET_STATS_MAX_COUNT);
    printf("%-15s%-15s%-15s%-15s%-15s%-15s%-15s\n", "Num", "ID", "State", "Proto", "Sent", "Recv", "Time");
    while (COMPLETED_FLAG != threadFlag.get()) {
        count = SocketStats::mbed_stats_socket_get_each(&stats[0], MBED_CONF_NSAPI_SOCKET_STATS_MAX_COUNT);
        for (int i = 0; i < count; i++) {
            stdio_mutex.lock();
            printf("\n%-15d", num);
            printf("%-15p", stats[i].reference_id);

            switch (stats[i].state) {
                case SOCK_CLOSED:
                    printf("%-15s", "Closed");
                    break;
                case SOCK_OPEN:
                    printf("%-15s", "Open");
                    break;
                case SOCK_CONNECTED:
                    printf("%-15s", "Connected");
                    break;
                case SOCK_LISTEN:
                    printf("%-15s", "Listen");
                    break;
                default:
                    printf("%-15s", "Error");
                    break;
            }

            if (NSAPI_TCP == stats[i].proto) {
                printf("%-15s", "TCP");
            } else {
                printf("%-15s", "UDP");
            }
            printf("%-15d", stats[i].sent_bytes);
            printf("%-15d", stats[i].recv_bytes);
            printf("%-15lld\n", stats[i].last_change_tick);
            stdio_mutex.unlock();
        }
        num++;
        ThisThread::sleep_for(SAMPLE_TIME);
    }
    // Now allow the stats thread to simply exit by itself gracefully.
}

// Network interface
NetworkInterface *net;

int main()
{
    // Bring up the ethernet interface
    printf("Mbed OS Socket statistics example\n");

#ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version: %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif
    net = NetworkInterface::get_default_instance();

    if (!net) {
        printf("Error! No network interface found.\n");
        return 0;
    }

    nsapi_size_or_error_t result = net->connect();
    if (result != 0) {
        printf("Error! net->connect() returned: %d\n", result);
        return result;
    }

    // Show the network address
    const char *ip = net->get_ip_address();
    const char *netmask = net->get_netmask();
    const char *gateway = net->get_gateway();
    printf("IP address: %s\n", ip ? ip : "None");
    printf("Netmask: %s\n", netmask ? netmask : "None");
    printf("Gateway: %s\n", gateway ? gateway : "None");

    Thread *thread = new Thread(osPriorityNormal1, 2048);
    thread->start(print_stats);

    // Open a socket on the network interface, and create a TCP connection to api.ipify.org
    TCPSocket socket;
    // Send a simple http request
    char sbuffer[] = "GET / HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: close\r\n\r\n";
    nsapi_size_t size = strlen(sbuffer);

    result = socket.open(net);
    if (result != 0) {
        stdio_mutex.lock();
        printf("Error! socket.open() returned: %d\n", result);
        stdio_mutex.unlock();
    }

    int remaining = 256;
    int rcount = 0;
    char *buffer = new char[256];
    char *p = buffer;

    result = socket.connect("api.ipify.org", 80);
    if (result != 0) {
        stdio_mutex.lock();
        printf("Error! socket.connect() returned: %d\n", result);
        stdio_mutex.unlock();
        goto DISCONNECT;
    }

    // Loop until whole request sent
    while (size) {
        result = socket.send(sbuffer + result, size);
        if (result < 0) {
            stdio_mutex.lock();
            printf("Error! socket.send() returned: %d\n", result);
            stdio_mutex.unlock();
            goto DISCONNECT;
        }
        size -= result;
        stdio_mutex.lock();
        printf("sent %d [%.*s]\n", result, strstr(sbuffer, "\r\n") - sbuffer, sbuffer);
        stdio_mutex.unlock();
    }

    // Receive an HTTP response and print out the response line
    while (0 < (result = socket.recv(p, remaining))) {
        p += result;
        rcount += result;
        remaining -= result;
    }
    if (result < 0) {
        stdio_mutex.lock();
        printf("Error! socket.recv() returned: %d\n", result);
        stdio_mutex.unlock();
        goto DISCONNECT;
    }
    // the HTTP response code
    stdio_mutex.lock();
    printf("recv %d [%.*s]\n", rcount, strstr(buffer, "\r\n") - buffer, buffer);
    stdio_mutex.unlock();

    // The api.ipify.org service also gives us the device's external IP address
    p = strstr(buffer, "\r\n\r\n") + 4;
    stdio_mutex.lock();
    printf("External IP address: %.*s\n", rcount - (p - buffer), p);
    stdio_mutex.unlock();

DISCONNECT:
    delete[] buffer;

    // Close the socket to return its memory and bring down the network interface
    socket.close();

    // Bring down the ethernet interface
    net->disconnect();
    threadFlag.set(COMPLETED_FLAG);
    thread->join();
    delete thread;
    printf("Done\n");
}
