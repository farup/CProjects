# Alarm Clock 


## Introduction 
Unix- based alarm clock. The project is coded in C, with ubuntu WSL. The 
delivery consist of alarm.c and ring.c. Furthermore this document contains a short description for in total 3 
test, with a step-by-step guide in what and how. 


## My Solution - Quick Overview 

Alarms are scheduled by entering a desired time. The program forks, and the child process “sleeps”
the required time. Due to problems with playing audio in WSL, the child process instead executes a 
separate file, a.out, with the execl() system call function. This file prints “Ring!”. 

To avoid “zombies”, the child processes are checked on with the waitpid() system call after user
input, as described in the task


## Test 1 
**1. Compile program**
```bash
C:\Users\...>gcc ring.c 
C:\Users\...>gcc alarm.c -o alarm 
```

**2. Run program**
```bash
C:\Users\...>./alarm 
``` 


**3. Menu Select (schedule)**
```bash
C:\Users\...>s
``` 
**4. Schedule first alarm**
```bash
C:\Users\...>2022-03-28 23:59:59
``` 
Console: 
“Alarm Sucessfully added to list!”
“Scheduling alarm in XX seconds”. 
Program fork a new child process, which sleeps for the calculated time.

**5. Menu Select (list)**
```bash
C:\Users\...>l
```

```
"Alarm 1 at 2022-2-28 23:59:59 PID-Number"
```

**6. Schedule second alarm**
```bash
C:\Users\...>2022-03-29 23:59:59
``` 

## Test 2
... continue or repeat from test 1. 

**3. Menu Select (cancel)**
```bash
C:\Users\...>c
``` 
**4. What alarm to delete**
```bash
C:\Users\...>2
``` 

```
"Alarm 2 was successfully cancelled 
Process ID killed: XXXX"
```


**5. Menu Select (list) **
```bash
C:\Users\...>l
``` 

## Test 3 
Testing alarm ring and exit  

**1. Wait for cheduled alarm...** 
```
"--- RING! ---"
```

**2 Menu Select (exit)**
```bash
C:\Users\...>x
``` 












