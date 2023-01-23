# Protocol Version 1

The specification for the MeatMe application protocol, both server and client.

COMMUNICATION
=============

Done in **4** general stages.

## Example:

`Host1` has key `X`, `Host2` has key `Y`.
`X` and `Y` are not shared in any way, neither host knows the other's key.

* `Host1` writes `Packet`, sends `Packet` adding `X`
* `Host2` receives `Packet`+`X`, resends `Packet`+`X` adding `Y`
* `Host1` receives `Packet`+`X`+`Y`, resends `Packet`+`X`+`Y` subtracting `X`
* `Host2` receives `Packet`+`Y`, reads `Packet` subtracting `Y`

The way they are applied doesn't matter, the only thing that matters is that the host is able to remove the key it applied on the packet, without affecting the packet.

The best way to make sure the packet isn't affected by applying and removing the key is by using linear methods to lock it, meaning subtraction or addition per byte, making sure to handle overflows. see `applyKey()` in main.go.

PACKETS
=======

Packets are no more than `PacketSizeMax`(see [COMPLEX STRUCTURES AND CONSTANTS](#complex-structures-and-constants))

## Packet structure:
```go
Version uint16
EntriesNum uint8
Data []byte/uint8
```

* `Version` is the protocol version of the packet's host, sent to make sure weare speaking the same language here.
* `EntriesNum` is mainly to help detect MITM attacks, since if the actual number of entries doesn't match `EntriesNum` we can safely kill the communication. Obviously it's not a full proof method, but rather a part of a broader method to deal with MITM.
* `Data` consists of entries that come one after the other:
	```go
	EntryType byte
	EntryData []byte
	```

ENTRY TYPES
===========

They sperate into two groups, server and client entries, each one has their own `EntryType` value space, meaning for instance `EntryType`=42 from a server doesn't represent the same data as `EntryType`=42 for the client, though probably it represents the same type.

Client entries are obviously purely requests, and server entries are responses to those requests.

COMPLEX STRUCTURES AND CONSTANTS
================================

Complex structures that are reused in entries and constants that may change
in futrue versions of the protocol.
```go
SelfieSize = 64 // The width/height of a selfie.
PacketSizeMax = 4096 // The maximum size of the packet.
```

ENTRIES
=======

Each definition will be given a number after it's name, which represents it's `EntryType` value: `Name=EntryType integer`.

---------------------------

```go
IdentifyEntry=0 // Client & Server
```
Sent when the client attempts to log in, requests the server to identify them.

## Client:
```go
Selfie []byte
```
* `NewUser` implies that we are a new user.
* `Selfie` is an array of pixels in RGB888 format. With it's width and height being equal to `SelfieSize`.

## Server:
```go
Success byte
YouExist byte
Class byte
```
* `Success` is .
* `YouExist` is a boolean value that represents if the user was or was not found in the database respectively.
* `Class` includes: 0 for Femboy, 1 for Male, 2 for Alien.

---------------------------

```go
SetupEntry=1 // Client & Server
```
Sent after usccessful ID.

## Client:
```go
Name []byte
Tags []byte
```
* `Name` is an ASCII null terminated string, the name is automatically formatted by the server to have a capital letter in the beginning, only the first word, 
* `Tags` is an array of ASCII null terminated strings, each one is seperated by a null terminator, example: `Games\0Stuff\0More stuff\0`. 

## Server:
```go
// EntryType only, no EntryData.
```
Entry is empty, it confirms.

---------------------------
```go
CompleteTag=1 // Client & Server
```
Sent after usccessful ID.

## Client:
```go
Tag []byte
```
* `Name` is an ASCII null terminated string, the name is automatically formatted by the server to have a capital letter in the beginning, only the first word, 
* `Tags` is an array of ASCII null terminated strings, each one is seperated by a null terminator, example: `Games\0Stuff\0More stuff\0`. 

## Server:
```go
BestTags []byte
```

