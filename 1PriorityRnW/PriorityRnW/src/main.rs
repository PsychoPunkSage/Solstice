use std::{
    io::stdin,
    sync::{Arc, Condvar, Mutex},
    thread,
};

struct SharedState {
    shared_val: i32,
    readers_count: i32,
    writers_count: i32,
}

fn reader(state: Arc<(Mutex<SharedState>, Condvar, Condvar)>) {}
fn writer(state: Arc<(Mutex<SharedState>, Condvar, Condvar)>) {}

fn main() {
    println!("Enter no. of Readers:");
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    let n_readers: i32 = input.trim().parse().unwrap();

    println!("Enter no. of Writers:");
    input.clear();
    std::io::stdin().read_line(&mut input).unwrap();
    let n_writers: i32 = input.trim().parse().unwrap();

    // STATE Init
    let state = Arc::new((
        Mutex::new(SharedState {
            shared_val: 0,
            readers_count: 0,
            writers_count: 0,
        }),
        Condvar::new(),
        Condvar::new(),
    ));

    // Thread Handlers
    let mut readers = Vec::new();
    let mut writers = Vec::new();

    // THREAD CREATION
    (0..n_readers).into_iter().for_each(|i| {
        let c_state = Arc::clone(&state);
        let handle = thread::spawn(move || {
            reader(c_state);
        });

        readers.push(handle);
    });

    (0..n_writers).into_iter().for_each(|i| {
        let c_state = Arc::clone(&state);
        let handle = thread::spawn(move || {
            writer(c_state);
        });
        writers.push(handle);
    });

    // JoinHandles
    readers
        .into_iter()
        .for_each(|handle| handle.join().unwrap());

    writers
        .into_iter()
        .for_each(|handle| handle.join().unwrap());

    println!("Bye Bye!!!");
}
