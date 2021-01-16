# slow_receiver

This repository contains an example to show some behaviour of (unix domain) socket regarding slow connections.

## Background

In another project, two processes should share some data using unix domain sockets (aka local sockets).  
Therefore we had the idea to use [poll](https://man7.org/linux/man-pages/man2/poll.2.html) to determine when
the socket becomes readable but to write data regardless of the socket is writable.

This repository serves as a playground for that idea and shows its outcome.

## Build and Run

````
meson build
cd build
meson compile
./slow_receiver &
time ./blocking_sender
time ./nonblocking_sender
time ./nonblocking_poll_sender
````

## Blocking Sender

In this example, the sender uses an blocking socket and tries to write all the data at once.

````
> time ./blocking_sender
wrote 1048576 bytes

real    0m44,189s
user    0m0,000s
sys     0m0,004s
````

This approach might be acceptable when the only purpose of the sending process is to send data.

But one should be aware of the fact, that the application blocks until all data is written.

## Non-blocking Sender

In this example, the sender uses a non-blokcing socket and tries to write as much data as possible.
It uses a busy loop to write remaining data.

````
> time ./nonblocking_sender
wrote 219264 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 36544 bytes
wrote 25344 bytes

real    0m41,034s
user    0m10,799s
sys     0m30,217s
````

In my opinion, this approach is never acceptable, since the busy loop cosumes a lot of time.

## Non-blocking Sender using poll

In this example, the sender uses a non-blokcing socket and tries to write as much data as possible.
The poll function is used to determine if the socket is writable.

````
time ./nonblocking_poll_sender
wrote 219264 bytes
wrote 182720 bytes
wrote 182720 bytes
wrote 182720 bytes
wrote 182720 bytes
wrote 98432 bytes

real    0m44,595s
user    0m0,000s
sys     0m0,005s
````

This approach allows the application to work on other tasks multiplexed by poll.

## Conclusion

If the only purpose of the sending application is to write data, the approuach using blocking sockets should be considered.

If there are other tasks in sending application than writing data, non-blocking sockets using poll to determine writability should be considered.

Non-blocking sockets without poll to determine writablity should never be considered.
