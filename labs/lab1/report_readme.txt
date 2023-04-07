Brendan Yang  486295

Module Design
---
The initial requirements were met by creating two unisnged long, static variables named 'log_sec' and 'log_nsec' every 1 second.
The values were read into the program by passing the module_param() macro with the name of the variable, the type of the variable, and 0 to disallow any filesystem accesss to the variable.


Thread Design and Evaluation
---
To implement the thread 


Running module with default 1 sec recurrence
[ 3976.937982] initializing recurring timer module
[ 3976.938261] starting timer...
[ 3976.938291] ran spawn_func()
[ 3976.938309] iters: 0
[ 3976.938325] nvcsw: 1, nivcsw: 0
[ 3977.938299] waking task
[ 3977.938314] forwarding timer
[ 3977.938339] iters: 0
[ 3977.938355] nvcsw: 2, nivcsw: 0
[ 3978.938305] waking task
[ 3978.938323] forwarding timer
[ 3978.938387] iters: 0
[ 3978.938412] nvcsw: 3, nivcsw: 0

Running module with 1 sec and 500000000 ns (1.5 sec)

