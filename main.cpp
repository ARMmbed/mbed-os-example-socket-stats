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

#if !defined(MBED_CONF_NSAPI_SOCKET_STATS_ENABLED)
#error [NOT_SUPPORTED] Socket Statistics not supported
#endif

#define SAMPLE_TIME_MS 10
#define COMPLETED_FLAG (1UL << 0)

PlatformMutex stdio_mutex;
EventFlags threadFlag;

void print_socket_stats()
{
    mbed_stats_socket_t stats[MBED_CONF_NSAPI_SOCKET_STATS_MAX_COUNT];
    int count;

    memset(&stats[0], 0, sizeof(mbed_stats_socket_t) * MBED_CONF_NSAPI_SOCKET_STATS_MAX_COUNT);
    while (COMPLETED_FLAG != threadFlag.get()) {
        count = SocketStats::mbed_stats_socket_get_each(&stats[0], MBED_CONF_NSAPI_SOCKET_STATS_MAX_COUNT);
        for (int i = 0; i < count; i++) {
            stdio_mutex.lock();
            printf("ID: %p\n", stats[i].reference_id);

            switch (stats[i].state) {
                case SOCK_CLOSED:
                    printf("State: Closed\n");
                    break;
                case SOCK_OPEN:
                    printf("State: Open\n");
                    break;
                case SOCK_CONNECTED:
                    printf("State: Connected\n");
                    break;
                case SOCK_LISTEN:
                    printf("State: Listen\n");
                    break;
                default:
                    printf("State: Error\n");
                    break;
            }

            if (NSAPI_TCP == stats[i].proto) {
                printf("Protocol: TCP\n");
            } else {
                printf("Protocol: UDP\n");
            }
            printf("Sent bytes: %d\n", stats[i].sent_bytes);
            printf("Received bytes: %d\n", stats[i].recv_bytes);
            printf("Time in us: %lld\n\n", stats[i].last_change_tick);
            stdio_mutex.unlock();
        }
        ThisThread::sleep_for(SAMPLE_TIME_MS);
    }
    // Now allow the stats thread to simply exit by itself gracefully.
}

// Network interface
NetworkInterface *net;

int main()
{
    // Bring up the ethernet interface
    printf("Mbed OS Socket statistics example\n");

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
    SocketAddress a;
    net->get_ip_address(&a);
    printf("IP address: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
    net->get_netmask(&a);
    printf("Netmask: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
    net->get_gateway(&a);
    printf("Gateway: %s\n\n", a.get_ip_address() ? a.get_ip_address() : "None");

    Thread *thread = new Thread(osPriorityNormal1, 2048);
    thread->start(print_socket_stats);

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

    result = NetworkInterface::get_default_instance()->gethostbyname("api.ipify.org", &a);
    if (result != NSAPI_ERROR_OK) {
        printf("Error! DNS resolution failed with %d\n", result);
        goto DISCONNECT;
    }
    a.set_port(80);
    result = socket.connect(a);
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

    // The api.ipify.org service also gives us the device's external IP address
    p = strstr(buffer, "\r\n\r\n") + 4;

DISCONNECT:
    delete[] buffer;

    // Close the socket to return its memory and bring down the network interface
    socket.close();

    // Bring down the ethernet interface
    net->disconnect();
    threadFlag.set(COMPLETED_FLAG);
    thread->join();
    delete thread;
}
