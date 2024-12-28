use std::{
    io::{self, Read, Write},
    net::{TcpListener, TcpStream},
};

static DEFAULT_IP: &str = "127.0.0.1";
static DEFAULT_PORT: u16 = 8000;
static BUFFER_SIZE: usize = 1024;

fn create_server(ip_addr: &str, port: u16) -> io::Result<(TcpListener)> {
    println!("Creating Socket");
    let listener = TcpListener::bind((ip_addr, port))?;
    println!("Socket created successfully");
    Ok(listener)
}

fn handle_client(mut stream: TcpStream) -> io::Result<()> {
    println!("Client connected");
    let addr = stream.peer_addr()?;
    println!("Socket created successfully <{}>", addr);

    let mut buffer = [0; BUFFER_SIZE];

    loop {
        let bytes_read = stream.read(&mut buffer)?;
        if bytes_read == 0 {
            println!("Client disconnected");
            break;
        }

        let received = String::from_utf8_lossy(&buffer[..bytes_read]);
        println!("Received: {}", received);

        let resp = received.to_uppercase();
        stream.write_all(resp.as_bytes())?;
    }

    Ok(())
}

fn main() {
    let args: Vec<String> = std::env::args().collect();

    let ip_addr = args.get(1).map(|s| s.as_str()).unwrap_or(DEFAULT_IP);
    let port = args
        .get(2)
        .and_then(|a| a.parse::<u16>().ok())
        .unwrap_or(DEFAULT_PORT);

    let server = 
}
