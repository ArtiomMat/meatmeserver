package main

import (
	"encoding/binary"
	"log"
	"math/rand"
	"net"
	"time"
)

const port = "6969"
const protocolVersion uint16 = 1
const keySize = 4

type link struct {
	first bool // First time interacting?

	writing bool // Are we waiting to stage #2 of writing to the link
	reading bool // Are we waiting to stage #2 of reading from the link

	key [keySize]byte // The private key we use to encrypt the packet on our end

	t uint32 // Last time of any kind of interaction, including writes from us.
}

func (l *link) updateTime() {
	l.t = uint32(time.Now().Unix())
}

var links = make(map[net.Addr]*link)

var listener net.PacketConn

// factor must be either -1 or 1, it is applied to the key.
// 1 means encryption and -1 decryption, or whatever you want.
func applyKey(b []byte, n int, key [keySize]byte, factor int) {
	for i, j := 0, 0; i < n; i++ {
		if j == keySize {
			j = 0
		}

		// Overflow proofing
		key := factor * int(key[j])
		tested := int(b[i]) + key
		if tested > 255 {
			b[i] = byte(key - tested - 255)
		} else if tested < 0 {
			b[i] = byte(255 - key + tested)
		}

		j++
	}
}

func newLink() *link {
	var l link
	l.first = true

	l.writing = false
	l.reading = false

	for i := 0; i < keySize; i++ {
		l.key[i] = byte(rand.Intn(255))
	}

	return &l
}

func writeTo(linkPtr *link, p []byte, addr net.Addr) {
	linkPtr.writing = true

	applyKey(p, len(p), linkPtr.key, 1)
	listener.WriteTo(p, addr)

	linkPtr.updateTime()
}

// p must be sliced
func parse(linkPtr *link, p []byte, addr net.Addr) {
	version := binary.BigEndian.Uint16(p[0:2])
	if version != protocolVersion {
		delete(links, addr)
	}

}

func main() {
	listener, e := net.ListenPacket("udp", ":"+port)
	if e != nil {
		log.Fatal(e)
	}
	defer listener.Close()

	p := make([]byte, 4096)

	for {
		n, addr, e := listener.ReadFrom(p)
		if e != nil {
			log.Println(e)
			continue
		}

		linkPtr, exists := links[addr]

		if !exists {
			links[addr] = newLink()
		} else {
			linkPtr.first = false
		}

		if linkPtr.writing { // Stage #2 of writing
			linkPtr.writing = false

			applyKey(p, n, linkPtr.key, -1)
			listener.WriteTo(p[0:n], addr)

			linkPtr.updateTime()
		} else if linkPtr.reading { // Stage #2 of reading
			linkPtr.reading = false

			applyKey(p, n, linkPtr.key, -1)

			linkPtr.updateTime()
			go parse(linkPtr, p[0:n], addr)
		} else { // Stage #1 of reading
			linkPtr.reading = true

			applyKey(p, n, linkPtr.key, 1)
			listener.WriteTo(p[0:n], addr)
		}
	}
}
