use std::{
    io::{self, BufRead, Read, Write},
    net::TcpStream,
};

const DEFAULT_IP: &str = "127.0.0.1";
const DEFAULT_PORT: u16 = 8080;
const BUFFER_SIZE: usize = 1024;

fn setup_connection(ip_addr: &str, port: u16) -> io::Result<TcpStream> {
    println!("Attempting to connect to {}:{}", ip_addr, port);
    let conn = TcpStream::connect((ip_addr, port)).unwrap();
    println!("Connected Successfully");
    Ok(conn)
}

fn send_messages(stream: &mut TcpStream, message: &str) -> io::Result<()> {
    stream.write_all(message.as_bytes()).unwrap();
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

fn main() {}
