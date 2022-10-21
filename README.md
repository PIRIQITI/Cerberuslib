# CERBERUSLIB LIBRARY

## DESCRIPTION:
This library is based on Zed A. Shaw's liblcthw library.  
Library has many functions added to existing data structires and it also has singly linked list data structure added.  
It compiles as static and dynamic libraries ( see Makefile for more details ).  

## BUILD:
Type: `sudo make; sudo make install`. ( these will automatically run unit tests and install the library )  
You can run checks with "runchecks.sh" to look for dangerous C functions  
If you have valgrind it will write memory check results in tests.log you can check that out too  

## NOTES:
In header files every function has a comment above about what it does so if you don't understand something refer to those  
Every function which has `int` return type returns `CERB_OK ( 0 ) and CERB_ERR ( -1 )` so if you'll need use those defines  
Stack and Queue are derived from doubly linked list  

Lastly if there will be any bugs open a pull request and I'll take a look
