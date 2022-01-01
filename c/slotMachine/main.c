//RUNS ONLY ON LINUX
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<assert.h>

#define REELLEN 10      // number of max reel value
#define NUM_REELS 3     // number of reels to be shown
#define SLEEP_MILSEC 100*1000   //microSec -> milSec
#define clearConsole() printf("\033[H\033[J")

typedef struct{
    int currentPoint;
    int reel[REELLEN];
}Reel;

Reel *ConstructReel(int);
void goAheadReel(Reel *);
int getCurrentReelValue(Reel *);
void showCurrentDisplay(Reel *[]);
int *getCurrentDisplay(Reel *[]);
void validate(void);

int main(){
    validate();
    Reel *reel = ConstructReel(rand()%REELLEN);
    for(int i = 0; i < REELLEN; i++){
        printf("%d", getCurrentReelValue(reel));
        goAheadReel(reel);
        printf(" next: %d", getCurrentReelValue(reel));
        printf("\n");
    }

    //create reel, init by random value
    Reel *reels[NUM_REELS];
    srand((unsigned int)time(NULL));
    for(int i = 0; i < NUM_REELS; i++){
        reels[i] = ConstructReel(rand()%REELLEN);
    }

    for(;;){
        //clearConsole();
        
        showCurrentDisplay(reels);
        
        //move reels point to next
        for(int i = 0; i < NUM_REELS; i++){
            goAheadReel(reels[i]);
        }

        usleep(SLEEP_MILSEC*10);
    }


}


//use as constructor
Reel *ConstructReel(int initPoint){
    Reel *reel = malloc(sizeof(Reel));
    //init current point
    reel->currentPoint = initPoint;
    //determine reel values
    for(int i = 0; i < REELLEN; i++){
        reel->reel[i] = i;
    }
}

void goAheadReel(Reel *reel){
    //if index is not reached to tail, currentPoint++
    if(!(reel->currentPoint+1 == REELLEN)){
        reel->currentPoint++;
    }else{
        reel->currentPoint = 0;
    }
}

void showCurrentDisplay(Reel *reels[]){
    printf("|");
    int *currentValues = getCurrentDisplay(reels);
    for(int i = 0; i < NUM_REELS; i++){
        printf(" %d |", currentValues[i]);
    }
    printf("\n");
}

int *getCurrentDisplay(Reel *reels[]){
    int currentDisplay[NUM_REELS];
    for(int i = 0; i < NUM_REELS; i++){
        currentDisplay[i] = getCurrentReelValue(reels[i]);
    }
    int *vp = &currentDisplay[0];
    return vp;
}

int getCurrentReelValue(Reel *reel){
    //printf("current reel val: %d", reel->reel[reel->currentPoint]);
    return reel->reel[reel->currentPoint];
}

void validate(){
    assert(NUM_REELS > 1);
    assert(REELLEN > 0);
}
