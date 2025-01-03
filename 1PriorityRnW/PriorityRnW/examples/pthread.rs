/*
use of `pthreads` directly in Rust is generally not recommended in Rust because:

    - It's unsafe - need to wrap everything in unsafe blocks
    - Will lose Rust's safety guarantees and ownership model benefits
    - It's more error-prone than using Rust's standard library abstractions
*/
use libc::{c_void, pthread_create, pthread_join, pthread_t, usleep};
use libc::{
    pthread_cond_broadcast, pthread_cond_destroy, /*pthread_cond_init, */ pthread_cond_t,
    pthread_cond_wait, pthread_mutex_destroy, /* pthread_mutex_init, */ pthread_mutex_lock,
    pthread_mutex_t, pthread_mutex_unlock, PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
};
use rand::Rng;
use std::ptr::null_mut;

#[used]
static mut SV: i32 = 0;
#[used]
static mut C_READERS: i32 = 0;
#[used]
static mut C_WRITERS: i32 = 0;

#[used]
static mut MUTEX: pthread_mutex_t = PTHREAD_MUTEX_INITIALIZER;
#[used]
static mut CV_READER: pthread_cond_t = PTHREAD_COND_INITIALIZER;
#[used]
static mut CV_WRITER: pthread_cond_t = PTHREAD_COND_INITIALIZER;

/*
The problem is that mutable references in Rust come with guarantees about exclusive access.
But with static mutable variables, multiple threads could access them simultaneously, breaking these guarantees.
*/

extern "C" fn readers(_arg: *mut c_void) -> *mut c_void {
    unsafe {
        let mut rng = rand::thread_rng();

        (0..20).into_iter().for_each(|_| {
            usleep(rng.gen_range(0..10));

            pthread_mutex_lock(&raw mut MUTEX);

            // If writers is executing then WAIT
            if C_WRITERS > 0 {
                pthread_cond_wait(&raw mut CV_READER, &raw mut MUTEX);
            }

            C_READERS += 1;
            println!("VAR(R): {}", *(&raw mut SV)); // SV ::> shared reference to mutable static is discouraged
            println!("    |_> Current Readers: {}", *(&raw mut C_READERS));
            println!("    |_> Current Writers: {}", *(&raw mut C_WRITERS));
            C_READERS -= 1;

            if C_READERS == 0 {
                pthread_cond_broadcast(&raw mut CV_WRITER);
            }

            pthread_mutex_unlock(&raw mut MUTEX);
        });

        println!("Reader is Exiting");
    }

    null_mut()
}

extern "C" fn writers(_arg: *mut c_void) -> *mut c_void {
    unsafe {
        let mut rng = rand::thread_rng();

        (1000..1020).into_iter().for_each(|i| {
            usleep(rng.gen_range(0..10));

            pthread_mutex_lock(&raw mut MUTEX);
            C_WRITERS += 1;

            while C_READERS > 0 {
                pthread_cond_wait(&raw mut CV_WRITER, &raw mut MUTEX);
            }

            SV = i;
            println!("VAR(W): {}", *(&raw mut SV));
            println!("    |_> Current Readers: {}", *(&raw mut C_READERS));
            println!("    |_> Current Writers: {}", *(&raw mut C_WRITERS));
            C_WRITERS -= 1;

            pthread_mutex_unlock(&raw mut MUTEX);
            pthread_cond_broadcast(&raw mut CV_READER);
            pthread_cond_broadcast(&raw mut CV_WRITER);
        });

        println!("Writer is Exiting");
    }

    null_mut()
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

    let mut readers_tid: Vec<pthread_t> = vec![0; n_readers as usize];
    let mut writers_tid: Vec<pthread_t> = vec![0; n_writers as usize];

    unsafe {
        // Create Readers thread
        (0..n_readers).into_iter().for_each(|i| {
            let mut tid = readers_tid[i as usize];
            if pthread_create(&mut tid, null_mut(), readers, null_mut()) != 0 {
                eprintln!("Unable to init Reader{}", i + 1);
                std::process::exit(1);
            }
            readers_tid[i as usize] = tid;
            println!("Reader{} initiated", i + 1);
        });

        // Create Writers thread
        (0..n_writers).into_iter().for_each(|i| {
            let mut tid = writers_tid[i as usize];
            if pthread_create(&mut tid, null_mut(), writers, null_mut()) != 0 {
                eprintln!("Unable to init Writer{}", i + 1);
                std::process::exit(1);
            }
            writers_tid[i as usize] = tid;
            println!("Writer{} initiated", i + 1);
        });

        // Join all threads
        readers_tid.into_iter().chain(writers_tid).for_each(|tid| {
            pthread_join(tid, null_mut());
        });

        // Cleanup
        pthread_mutex_destroy(&raw mut MUTEX);
        pthread_cond_destroy(&raw mut CV_READER);
        pthread_cond_destroy(&raw mut CV_WRITER);
    }
    println!("Bye Bye!!");
}
