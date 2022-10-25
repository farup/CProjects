#define _POSIX_SOURCE 200089L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define n 5

char action[1];
struct alarm 
{
    time_t time;
    int PID;
};

struct alarm alarms[n];
int arrayCounter = 0; // Counter for keeping track of postion in array. And if its full

void generateMenu() 
/*Function for printing welcome menu*/ 
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf( "Welcome to the alarm clock! It is currently %d-%d-%d %d:%d:%d. Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void printTime(time_t timeToPrint)
/* Function for printing time_t datatype in format YYYY-MM-DD hh:mm:ss */ 
{
    struct tm *timeinfo = localtime(&timeToPrint); // returns adress (pointer) of a struct tm
    printf("%d-%d-%d %d:%d:%d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

void listAlarms()
/* Function for listing all alarms */
{
    int length = sizeof(alarms)/(sizeof(alarms[0])); 
    for(int i = 0; i < length; i++){
        if(alarms[i].PID != 0) // Not listing "fake" alarms (not user generated)
        {
            printf("Alarm %i at ", i + 1); 
            printTime(alarms[i].time);
            printf("\t%i", alarms[i].PID);
            printf("\n");
        } 
    }
}

void addAlarm(struct alarm alarm)
/* Function for adding alarm struct to alarm array*/
{
    int length = sizeof(alarms)/sizeof(alarms[0]);
    if(arrayCounter <= length) // If still space in alarm array 
    {
        alarms[arrayCounter] = alarm; 
        arrayCounter++; 
        printf("Alarm Sucessfully added to list!\n");    
    }
}


void removeAlarm()
/* Function for removing and terminating alarm with userinput alarmIndex*/ 
{
    int alarmIndex; 
    printf("Cancel which alarm? ");
    scanf("%i", &alarmIndex); 

    int PIDToCancel = alarms[alarmIndex - 1].PID;
    int length = sizeof(alarms)/sizeof(alarms[0]);  

    for(int i = 0; i < length; i++){
        if (alarms[i].PID == PIDToCancel){
            kill(PIDToCancel, SIGKILL); // Killing alarm process
            printf("Alarm %i was successfully cancelled", alarmIndex); 
            alarms[i].time = NULL; // Overwriting array positon
            alarms[i].PID = 0; // Overwriting array positon
            printf("\t Process ID killed: %i\n", PIDToCancel);
        }
    }   
}

void removeByPID(int PIDToCancel)
/* Function for removing alarm from list by PID number */ 
{
    int length = sizeof(alarms)/sizeof(alarms[0]);
     for(int i = 0; i < length; i++){
        if (alarms[i].PID == PIDToCancel){
            alarms[i].time = NULL; 
            alarms[i].PID = 0; 
        }
    }
}

char generateUserInputMenu(); //declaration

int newAlarm()
/* Function for creating new alarm stuct */
{
    if(arrayCounter < n) // Checking if still space for new alarm
    {
        char newAlarmChar[25];
        struct tm converter = {0};
        char printTime[30]; 
        time_t enteredTime;
        int timeDifference; 

        printf("Enter the new alarm in format \"YYYY-MM-dd hh:mm:ss\" "); 
        scanf("%[^\n]24s", newAlarmChar);  // Reads input, with whitespaces
        
        strptime(newAlarmChar, "%Y-%m-%d %H:%M:%S", &converter); // Converting string to tm-structur
        enteredTime = mktime(&converter); // Creating time_c from tm structur. 
        
        timeDifference = difftime(enteredTime, time(NULL)); // Calculating sleep time 
        
        int pid = fork(); // Creating a child process. fork() returns 0 for child.  

        if(pid == 0){
            /*CHILD*/
            sleep(timeDifference); 
            execl("a.out",  NULL);  
            exit(0); // Terminates process   
        }
        else if (pid > 0) {
            /*PARENT*/
            struct alarm newAlarmStruct; // The new Alarm
            newAlarmStruct.time = enteredTime; // Alarm time
            newAlarmStruct.PID = pid; // Child process id 
            addAlarm(newAlarmStruct);
             
            printf("\tScheduling alarm in %i seconds\n", timeDifference); 
            generateUserInputMenu(); 
        }
    }
    else{
        printf("Array is full. Not able to creat new alarm!\n");
    }
    return 0;    
}

char generateUserInputMenu()
/* Function for userinput selection */
{
    char action = NULL; 
    int pidToZombie; 
    printf("\n>>>");
    scanf("%s", &action);
    getchar(); // Cleaning buffer for scanf

    if(action == 's'){
        newAlarm();   
    }

    if(action == 'l'){
        listAlarms();  
    }

    if(action == 'c'){  
        removeAlarm();       
    }

    while((pidToZombie = waitpid(-1, NULL, WNOHANG)) > 0) // Checking on all children in order to not making zombies. WNOHANG is non bocking.
    {
       removeByPID(pidToZombie);  
    }
    
    return action;       
}


int main()
{

    int pidToZombie; 
    generateMenu();
    while(generateUserInputMenu() != 'x'){
        generateUserInputMenu(); 
    }

    sleep(1); 
    printf("Goodbye!\n"); 

    return 0;
}