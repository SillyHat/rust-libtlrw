## rust-libtlrw
TLRW algorithm implementation for the Rust programming language  
*Under construction*
:dolls:

**What do we have now:**
- Basic implementation of TLRW algorithm: the byte-lock structure and main operations. Written on C, linked to Rust. Works fine.
- C library with some atomic operations implementation, based on inline assembly code for x86/x86_64.
- Some kind of test program that basically does nothing.

**What needs to be done:**
- Test stuff! We need a working program to test and compare to other synchronization primitives.
- Some wrapper for operations with a bytelock structure. No explicit bytelock operations, we need to make our interface high-level.
- An algorithm for handling the "rollback due to time-out" situations. *It ain't a transaction if it ain't rolling back*.
- Avoid Rust compiler censoring our actions!
