# Ring3 Thread Safety Checks for Windows ![GitHub](https://img.shields.io/github/license/illegal-instruction-co/ThreadSafe?logo=ThreadSafe) ![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/illegal-instruction-co/ThreadSafe)


This code handles thread safety checks in primative level. 

## Purpose of the Code
The code aims to create a thread that continuously monitors its own safety by checking specific execution metrics. This is achieved by hooking into the SleepEx function from the Windows API to validate the state of the thread periodically. The primary objective is to detect anomalies such as thread termination or manipulation, which could indicate potential security threats or instability.

## Key Features
### Thread Creation and Monitoring:

The SafeThread class inherits from std::thread and initializes a new thread.
Upon initialization, the thread ID is stored, and hooks are set up using MinHook to intercept the SleepEx function.
### Hooked SleepEx Function:

The SleepExHook function is a custom implementation that replaces the original SleepEx function.
It checks if the thread is still running by verifying the thread's exit code and its state.
### RDTSC Checks:

The CheckRDTSC and FetchRDTSC methods are used to monitor the processor's timestamp counter (RDTSC).
These checks ensure that the thread's execution timing remains within expected bounds, helping to detect any unusual delays or interruptions.
### Reaction to Anomalies:

If an anomaly is detected (e.g., the thread is not found or terminated), the React method is triggered.
This method shows an error message and intentionally crashes the program to prevent further execution, indicating a critical issue with thread safety.
### Example Usage
In the main function, a SafeThread instance is created with a lambda function that includes periodic safety checks. The thread runs indefinitely, printing a "Thread is safe!" message every second. If any safety check fails, appropriate measures are taken to handle the anomaly.

This code is useful in scenarios where thread integrity is crucial, such as in security-sensitive applications or systems requiring high reliability and tamper detection.
