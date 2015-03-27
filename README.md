# utests
various linux toy tests

mem_release_test: tries to find out the threshold value above which memory 
is allocated with mmap() (opposed to sbrk())

sche: brings threads with different scheduler classes to the barrier and 
reports the order they wake up. Reqiures roor permissions.

Try <prog> -h for help 

