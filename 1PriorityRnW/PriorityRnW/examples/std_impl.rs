use rand::Rng;
use std::{
    sync::{Arc, Condvar, Mutex},
    thread,
    time::Duration,
};

struct SharedState {
    shared_val: i32,
    readers_count: i32,
    writers_count: i32,
}

fn reader(state: Arc<(Mutex<SharedState>, Condvar, Condvar)>) {
    let mut rng = rand::thread_rng();
    (0..20).into_iter().for_each(|_| {
        thread::sleep(Duration::from_millis(rng.gen_range(1..100))); // Random Sleep

        let (lock, reader_cv, writer_cv) = &*state;
        let mut guard = lock.lock().unwrap();

        // WAIT if there are writers
        while guard.writers_count > 0 {
            guard = reader_cv.wait(guard).unwrap();
        }

        guard.readers_count += 1;

        println!("VAR(R): {}", guard.shared_val);
        println!("    |_> Current Readers: {}", guard.readers_count);
        println!("    |_> Current Writers: {}", guard.writers_count);

        guard.readers_count -= 1;

        if guard.readers_count == 0 {
            // Broadcast....
            writer_cv.notify_all();
        }
    });

    println!("Reader is Exiting");
}
fn writer(state: Arc<(Mutex<SharedState>, Condvar, Condvar)>) {
    let mut rng = rand::thread_rng();

    (1000..1020).into_iter().for_each(|i| {
        thread::sleep(Duration::from_millis(rng.gen_range(1..100))); // Random Sleep

        let (lock, reader_cv, writer_cv) = &*state;
        let mut guard = lock.lock().unwrap();

        guard.writers_count += 1;

        // WAIT if there are Readers...
        while guard.readers_count > 0 {
            guard = writer_cv.wait(guard).unwrap();
        }

        guard.shared_val = i;

        println!("VAR(W): {}", guard.shared_val);
        println!("    |_> Current Readers: {}", guard.readers_count);
        println!("    |_> Current Writers: {}", guard.writers_count);

        guard.writers_count -= 1;

        reader_cv.notify_all();
        writer_cv.notify_all();
    });

    println!("Writer is Exiting");
}

fn main() {
    println!("Enter no. of Readers:");
    let mut input = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    let n_readers: i32 = input.trim().parse().unwrap();

    println!("Enter no. of Writers:");
    input.clear();
    std::io::stdin().read_line(&mut input).unwrap();
    let n_writers: i32 = input.trim().parse().unwrap();

    ////////////////
    // STATE Init //
    ////////////////
    let state = Arc::new((
        Mutex::new(SharedState {
            shared_val: 0,
            readers_count: 0,
            writers_count: 0,
        }),
        Condvar::new(), // reader cv
        Condvar::new(), // writer cv
    ));

    /////////////////////
    // Thread Handlers //
    /////////////////////
    let mut readers = Vec::new();
    let mut writers = Vec::new();

    /////////////////////
    // THREAD CREATION //
    /////////////////////
    (0..n_readers).into_iter().for_each(|_| {
        let c_state = Arc::clone(&state);
        let handle = thread::spawn(move || {
            reader(c_state);
        });

        readers.push(handle);
    });

    (0..n_writers).into_iter().for_each(|_| {
        let c_state = Arc::clone(&state);
        let handle = thread::spawn(move || {
            writer(c_state);
        });
        writers.push(handle);
    });

    /////////////////
    // JoinHandles //
    /////////////////
    readers
        .into_iter()
        .for_each(|handle| handle.join().unwrap());

    writers
        .into_iter()
        .for_each(|handle| handle.join().unwrap());

    println!("Bye Bye!!!");
}
