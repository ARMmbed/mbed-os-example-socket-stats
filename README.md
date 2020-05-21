![](./resources/official_armmbed_example_badge.png)
# Socket statistics Mbed OS example

This example demonstrates how you can collect statistics from network sockets on an Mbed OS Enabled platform.

Please refer to the [mbed-os-example-sockets](https://github.com/ARMmbed/mbed-os-example-sockets/blob/master/README.md) example for more information about network socket setup.

You can build the project with all supported [Mbed OS build tools](https://os.mbed.com/docs/mbed-os/latest/tools/index.html). However, this example project specifically refers to the command-line interface tool [Arm Mbed CLI](https://github.com/ARMmbed/mbed-cli#installing-mbed-cli).

1. Install Mbed CLI.
1. From the command-line, import the example: `mbed import mbed-os-example-socket-stats`
1. Change the current directory to where the project was imported.

## Application functionality

This application enables socket statistics by setting `nsapi.socket-stats-enabled` to true in `mbed_app.json`:

```
{
    "target_overrides": {
        "*": {
            "nsapi.socket-stats-enabled": true
        }
    }
}
```

The `main()` function opens a socket on the Ethernet interface and creates a TCP connection to generate some data traffic. It also starts a thread to periodically print socket statistics, such as `ID`, `State`, `Proto`, `Sent`, `Recv` and `Time` on the serial interface.

### Note

The current example is configured to use the Ethernet interface on supported devices. To use the example with a different interface, please follow the [socket example README](https://github.com/ARMmbed/mbed-os-example-sockets/blob/master/README.md).

## Building and running

1. Connect a USB cable between the USB port on the target and the host computer.
1. Connect the target Ethernet port to the internet.
1. Run this command to build the example project and program the microcontroller flash memory:

    ```bash
    $ mbed compile -m <TARGET> -t <TOOLCHAIN> --flash --sterm
    ```

(Note: You can use the Mbed CLI command-line option "--sterm" to open a serial terminal after flashing.)

Your PC may take a few minutes to compile your code.

The binary is located at `./BUILD/<TARGET>/<TOOLCHAIN>/mbed-os-example-socket-stats.bin`.

Alternatively, you can manually copy the binary to the target, which gets mounted on the host computer through USB.

Depending on the target, you can build the example project with the `GCC_ARM`, `ARM` or `IAR` toolchain. After installing Arm Mbed CLI, run the command below to determine which toolchain supports your target:

```bash
$ mbed compile -S
```

## Expected output

The serial terminal shows an output similar to:

```
--- Terminal on /dev/ttyACM0 - 9600,8,N,1 ---
Mbed OS Socket statistics example
IP address: 10.2.202.163
Netmask: 255.255.255.0
Gateway: 10.2.202.1

Iteration: 1    ID: 0x2000e208    State: Open      Proto: TCP    Sent: 0    Recv: 0    Time: 7610
Iteration: 1    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 2    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 2    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 3    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 3    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 4    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 4    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 5    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 5    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 6    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 6    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 7    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 7    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 8    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 8    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 9    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 9    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 10    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 10    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 11    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 11    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 12    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 12    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 13    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 13    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 14    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 14    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
Iteration: 15    ID: 0x2000e208    State: Connected Proto: TCP    Sent: 58    Recv: 0    Time: 7815
Iteration: 15    ID: 0x2000e050    State: Closed    Proto: UDP    Sent: 31    Recv: 257    Time: 7615
End of Mbed OS Socket statistics example
```

The information below shows how to interpret the above fields:

```
Iteration:      Number of times socket state changes
ID:             Socket id
State:          Socket states
Protocol:       TCP, UDP
Sent bytes:     Number of bytes sent through this socket  
Received bytes: Number of bytes received through this socket
Time in us:     When state last changed
```

## Troubleshooting 

If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it. 

## Related links

- [Mbed OS Network interfaces](https://os.mbed.com/docs/latest/apis/network-interfaces.html)
- [Mbed OS network socket API](https://docs.mbed.com/docs/mbed-os-api-reference/en/latest/APIs/communication/network_sockets/).
- [Mbed OS socket stats API](https://os.mbed.com/docs/latest/apis/socketstats.html).
- [Mbed OS configuration](https://os.mbed.com/docs/latest/reference/configuration.html).
- [Mbed OS serial communication](https://os.mbed.com/docs/latest/tutorials/serial-communication.html).
- [Mbed boards](https://os.mbed.com/platforms/).

### License and contributions

The software is provided under the Apache-2.0 license. Contributions to this project are accepted under the same license. Please see contributing.md for more information.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
