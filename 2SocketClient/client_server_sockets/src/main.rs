use std::{
    io::{self, BufRead, Read, Write},
    net::TcpStream,
};

const DEFAULT_IP: &str = "127.0.0.1";
const DEFAULT_PORT: u16 = 8000;
const BUFFER_SIZE: usize = 1024;

fn setup_connection(ip_addr: &str, port: u16) -> io::Result<TcpStream> {
    println!("Attempting to connect to {}:{}", ip_addr, port);
    let conn = TcpStream::connect((ip_addr, port))?;
    println!("Connected Successfully");
    Ok(conn)
}

fn send_messages(stream: &mut TcpStream, message: &str) -> io::Result<()> {
    stream.write_all(message.as_bytes())?;
    println!("Message Sent: {}", message);
    Ok(())
}
fn receive_response(stream: &mut TcpStream) -> io::Result<String> {
    let mut buffer = [0; BUFFER_SIZE];
    let bytes_read = stream.read(&mut buffer)?;
    let response = String::from_utf8_lossy(&buffer[..bytes_read]).to_string();

    println!("Server Response: {}", response);
    Ok(response)
}

fn communication_loop(mut stream: TcpStream) -> io::Result<()> {
    let mut input = String::new();
    let mut stdin_lock = io::stdin().lock();

    loop {
        println!("Enter message to be sent:");
        io::stdout().flush()?;

        input.clear();
        stdin_lock.read_line(&mut input)?;
        let msg = input.trim();

        if msg == "exit" {
            println!("Exiting...");
            break;
        }

        send_messages(&mut stream, msg)?;
        receive_response(&mut stream)?;
    }
    Ok(())
}

fn main() -> io::Result<()> {
    let args: Vec<String> = std::env::args().collect();

    let ip_addr = args.get(1).map(|s| s.as_str()).unwrap_or(DEFAULT_IP);
    let port = args
        .get(2)
        .and_then(|s| s.parse::<u16>().ok())
        .unwrap_or(DEFAULT_PORT);

    let stream = setup_connection(ip_addr, port)?;
    println!("Connected to {}:{}", ip_addr, port);

    communication_loop(stream)?;

    Ok(())
}
