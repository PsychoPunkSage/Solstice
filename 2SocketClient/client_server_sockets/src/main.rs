use std::{
    io::{self, Write},
    net::TcpStream,
};

const DEFAULT_IP: &str = "127.0.0.1";
const DEFAULT_PORT: u16 = 8080;
const BUFFER_SIZE: usize = 1024;

// fn setup_connection(ip_addr: &str, port: u16) -> io::Result<TcpStream> {}

fn send_messages(stream: &mut TcpStream, message: &str) -> io::Result<()> {
    stream.write_all(message.as_bytes()).unwrap();
    println!("Message Sent: {}", message);
    Ok(())
}
fn receive_response(stream: &mut TcpStream) /*-> io::Result<String>*/
{
    let mut buffer = [0; BUFFER_SIZE];
    let bytes_read = stream.read(&mut buffer)?;
    let response = String::from_utf8_lossy(&buffer[..bytes_read]).to_string();
}
fn main() {}
