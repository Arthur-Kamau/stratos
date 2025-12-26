// Net module - Low-level networking (TCP/UDP sockets)
// Inspired by Go's net package

package net;

// ============================================================================
// Types
// ============================================================================

enum SocketType {
    TCP,
    UDP,
    UNIX
}

class Conn {
    var fd: int;
    var addr: string;
    var port: int;

    fn read(buffer: Array<byte>) int;
    fn write(data: Array<byte>) int;
    fn readString(maxBytes: int) string;
    fn writeString(data: string) int;
    fn close() void;
    fn setTimeout(ms: int) void;
}

class Listener {
    var fd: int;
    var addr: string;
    var port: int;

    fn accept() Result<Conn, Error>;
    fn close() void;
}

class UDPConn {
    var fd: int;
    var addr: string;
    var port: int;

    fn readFrom(buffer: Array<byte>) Pair<int, string>;
    fn writeTo(data: Array<byte>, addr: string, port: int) int;
    fn close() void;
}

class IPAddr {
    var ip: string;
    var port: int;

    fn toString() string;
    fn isIPv4() bool;
    fn isIPv6() bool;
}

// ============================================================================
// Native function declarations
// ============================================================================

// TCP
fn dial(network: string, address: string) Result<Conn, Error>;
fn dialTCP(addr: string, port: int) Result<Conn, Error>;
fn listen(network: string, address: string) Result<Listener, Error>;
fn listenTCP(addr: string, port: int) Result<Listener, Error>;

// UDP
fn dialUDP(addr: string, port: int) Result<UDPConn, Error>;
fn listenUDP(addr: string, port: int) Result<UDPConn, Error>;

// Address parsing
fn parseIP(s: string) Result<IPAddr, Error>;
fn resolveTCPAddr(addr: string) Result<IPAddr, Error>;
fn resolveUDPAddr(addr: string) Result<IPAddr, Error>;

// DNS
fn lookupHost(host: string) Result<Array<string>, Error>;
fn lookupIP(host: string) Result<Array<IPAddr>, Error>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Simple TCP server
class TCPServer {
    var listener: Listener;
    var handler: Function<Conn, void>;
    var running: bool;

    constructor(addr: string, port: int, handleFn: Function<Conn, void>) {
        val listenerResult = listenTCP(addr, port);
        if !listenerResult.ok() {
            panic("Failed to create TCP server: " + listenerResult.err().message);
        }

        this.listener = listenerResult.unwrap();
        this.handler = handleFn;
        this.running = false;
    }

    fn start() void {
        this.running = true;

        while this.running {
            val connResult = this.listener.accept();

            if connResult.ok() {
                val conn = connResult.unwrap();

                // Handle connection in goroutine
                concurrent.go(() => {
                    this.handler(conn);
                    conn.close();
                });
            }
        }
    }

    fn stop() void {
        this.running = false;
        this.listener.close();
    }
}

fn newTCPServer(addr: string, port: int, handler: Function<Conn, void>) TCPServer {
    return TCPServer(addr, port, handler);
}

// Simple TCP client
fn connectTCP(addr: string, port: int) Result<Conn, Error> {
    return dialTCP(addr, port);
}

// Read line from connection
fn readLine(conn: Conn) string {
    var result = "";
    var buffer = Array<byte>(1);

    while true {
        val n = conn.read(buffer);
        if n == 0 {
            break;
        }

        val char = buffer[0];
        if char == '\n' {
            break;
        }

        result = result + char;
    }

    return strings.trim(result);
}

// Write line to connection
fn writeLine(conn: Conn, line: string) int {
    return conn.writeString(line + "\n");
}

// Copy data from one connection to another
fn copy(dst: Conn, src: Conn) int {
    var total = 0;
    var buffer = Array<byte>(4096);

    while true {
        val n = src.read(buffer);
        if n == 0 {
            break;
        }

        dst.write(buffer);
        total = total + n;
    }

    return total;
}

// Pipe two connections (bidirectional)
fn pipe(conn1: Conn, conn2: Conn) void {
    val wg = concurrent.newWaitGroup();
    wg.add(2);

    concurrent.go(() => {
        copy(conn1, conn2);
        wg.done();
    });

    concurrent.go(() => {
        copy(conn2, conn1);
        wg.done();
    });

    wg.wait();
}

// UDP echo server
class UDPServer {
    var conn: UDPConn;
    var handler: Function<Array<byte>, string, int, Array<byte>>;
    var running: bool;

    constructor(addr: string, port: int, handleFn: Function<Array<byte>, string, int, Array<byte>>) {
        val connResult = listenUDP(addr, port);
        if !connResult.ok() {
            panic("Failed to create UDP server: " + connResult.err().message);
        }

        this.conn = connResult.unwrap();
        this.handler = handleFn;
        this.running = false;
    }

    fn start() void {
        this.running = true;
        var buffer = Array<byte>(4096);

        while this.running {
            val result = this.conn.readFrom(buffer);
            val bytesRead = result.first;
            val clientAddr = result.second;

            if bytesRead > 0 {
                val response = this.handler(buffer, clientAddr, bytesRead);
                this.conn.writeTo(response, clientAddr, 0);
            }
        }
    }

    fn stop() void {
        this.running = false;
        this.conn.close();
    }
}

fn newUDPServer(addr: string, port: int, handler: Function<Array<byte>, string, int, Array<byte>>) UDPServer {
    return UDPServer(addr, port, handler);
}

// Check if host is reachable
fn isReachable(host: string, port: int, timeoutMs: int) bool {
    val connResult = dialTCP(host, port);

    if connResult.ok() {
        val conn = connResult.unwrap();
        conn.close();
        return true;
    }

    return false;
}

// Get local IP addresses
fn getLocalIPs() Array<string> {
    val hostname = os.getHostname();
    val ipsResult = lookupHost(hostname);

    if ipsResult.ok() {
        return ipsResult.unwrap();
    }

    return Array<string>();
}
