# Peer 2 PoW Proof of Concept

This example shows how to send/receive Nano using myNanoJS with P2PoW API

## CAUTION:

- Don't use any private keys or SEED in this example, create a new one SEED to send and receive NANOS;
- Don't use your SEED in this example
- Send only small amount of Nanos for testing. You can send very little amount to test such as 0.0003 Nanos for example

## POC

- Request P2PoW information
- Checks if fee is <= MAX_FEE
- Create Nano Block (receive or send Nanos)
- Create P2PoW block
- Sign P2PoW block
- Parse P2PoW block to JSON
- Request work

## Before you install

## Installing

```sh
git clone https://github.com/devfabiosilva/myNanoJS
```

```sh
cd examples
```

```sh
npm install
```

```sh
cd 01-p2pow
```

```sh
node index.js
```

## License

MIT

