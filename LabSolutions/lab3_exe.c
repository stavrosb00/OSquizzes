
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <limits.h>


//askhsh3 OS
//-----------------------------------------------
//https://stackoverflow.com/questions/9148670/how-to-fork-n-child-processes-correctly-in-c/9148694

void minmaxAllListsFork( int **numbers, int nList, int *nElem ){
    //pid_t pid;
    //int minmax[2];
    //minmax[0] = INT_MAX; //0 gia elaxisth timh
    //minmax[1] = INT_MIN; //1 gia megisth timh
    int i,pid;
    //fork() nList 8ygatrikes
    for(i = 0; i < nList; i++) {
    pid = fork();
    if(pid < 0) { //error
        exit(1);
    } else if (pid == 0) { //mpainw 8ygatrikh
        //algori8mo 2o bullet
        int minmax[2];
        minmax[0] = INT_MAX; //0 gia elaxisth timh
        minmax[1] = INT_MIN; //1 gia megisth timh
        char buffer_name[8+i]; //minmax- + 1offset+ i
        int k;
        for(k = 0 ; k < nList ; k++){
                int j;
                for(j = 0 ; j < nElem[k]; j++){
                    if ( minmax[0] > numbers[k][j]){
                        minmax[0] = numbers[k][j];
                    }else if(minmax[1] < numbers[k][j]){
                        minmax[1] = numbers[k][j];
                    }
                }
                sprintf(buffer_name,"minmax-%d",i);
                writeBinary(buffer_name, minmax);
            }
        exit(0); 
    } else  { //3o bullet gia mhn exw ki alla spawns mexri na teleiwsoun oles kai katalhksw anadromika me 2^n diergasies
        wait(NULL);
    }
}



    
    return;
    
}