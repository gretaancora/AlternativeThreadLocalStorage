# Alternative Thread Local Storage (TLS)

This homework deals with the implementation of a TLS support standing aside of the original one offered by gcc. It should be based on a few macros with the following meaning:
1. PER_THREAD_MEMORY_START and PER_THREAD_MEMORY_END are used to delimitate the variable declarations that will be part of TLS
2. READ_THREAD_VARIABLE(name) is used to return the value of a TLS variable with a given name
3. WRITE_THREAD_VARIABLE(name, value) is used to update the TLS variable with a given name to a given value 

Essentially, the homework is tailored to implementing an interface for managing per-thread memory resembling the one offered by the Linux kernel for managing per-CPU memory. 
