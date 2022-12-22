# Interprocess-Communication

This assignment implements three kinds of interprocess communication to send a list of 50 strings by breaking them into packets of 10 each, sending them with their indices, and waiting for the greatest index to be returned to send the next index.

## Installation

Clone this repo, `cd` into it, and run:

```console
make
```

This will generate the binaries for all three IPC methods.

## Usage

You can run the Socket IPC using
```console
./P1socket & ./P2socket
```

You can run the FIFO IPC using
```console
./P1fifo & ./P2fifo
```

You can run the Shared Memory IPC using
```console
./P1shared & ./P2shared
```

## References

[Beej's Guide to UNIX IPCs](https://beej.us/guide/bgipc/)