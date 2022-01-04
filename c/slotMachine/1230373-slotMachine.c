//RUNS ONLY ON LINUX
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<assert.h>
#include<pthread.h>

#define REELLEN 10      // number of max reel value like {0, 1, 2, ..., REELLEN-1}
#define NUM_REELS 3     // number of reels to be shown in a slotMachine
#define SLEEP_MILSEC 1000*1000   //microSec -> milSec
#define clearConsole() printf("\033[H\033[J")
#define ASCII_ENTER 10

typedef struct{
    int currentPoint;
    int reel[REELLEN];
}Reel;

//for pthread arg
typedef struct{
    int isAllPushed;        //use as boolean, 0 = false, else = true
    Reel *reels[NUM_REELS];
}SlotMachine;

Reel *ConstructReel(int);
SlotMachine *ConstructSlotMachine(Reel *[]);
void goAheadReels(Reel *[]);
void goAheadReel(Reel *);
int getCurrentReelValue(Reel *);
void showCurrentDisplay(Reel *[]);
int *getCurrentDisplay(Reel *[]);
void validate(void);
int showResult(Reel *[]);
//for threads
void* asyncKeyMonitor(void *);
void* asyncReelsRotator(void *);
void fixAllReelValue(Reel *, int);

int main(){
    validate();
    //create reel, init by random value
    Reel *reels[NUM_REELS];
    srand((unsigned int)time(NULL));
    for(int i = 0; i < NUM_REELS; i++){
        reels[i] = ConstructReel(rand()%REELLEN);
    }

    SlotMachine *slotMachine = ConstructSlotMachine(reels);
    //rotate reels, await keyboard hits
    pthread_t awaitKeyboardHits;
    pthread_t showingThread;
    pthread_create(&awaitKeyboardHits, NULL, asyncKeyMonitor, slotMachine);
    pthread_create(&showingThread, NULL, asyncReelsRotator, slotMachine);
    pthread_join(awaitKeyboardHits, NULL);
    pthread_join(showingThread, NULL);

    return showResult(reels);
}

//use as constructor
Reel *ConstructReel(int initPoint){
    Reel *reel = malloc(sizeof(Reel));
    //init current point
    reel->currentPoint = initPoint;
    //determine reel values like {0, 1, 2, 3, 4, 5, ...}
    for(int i = 0; i < REELLEN; i++){
        reel->reel[i] = i;
    }
    return reel;
}

SlotMachine *ConstructSlotMachine(Reel *reels[]){
    SlotMachine *slotMachine = malloc(sizeof(SlotMachine));
    for(int i = 0; i < NUM_REELS; i++){
        slotMachine->reels[i] = reels[i];
    }
    slotMachine->isAllPushed = 0;
    return slotMachine;
}

//move multiple reels ahead
void goAheadReels(Reel *reels[]){
    for(int i = 0; i < NUM_REELS; i++){
        goAheadReel(reels[i]);
    }
}

//move one single reel ahead
void goAheadReel(Reel *reel){
    //if index is not reached to tail, currentPoint++
    if(!(reel->currentPoint+1 == REELLEN)){
        reel->currentPoint++;
    }else{
        reel->currentPoint = 0;
    }
}

//displaying current reels like | 1 | 2 | 3 |
void showCurrentDisplay(Reel *reels[]){
    printf("|");
    int *currentValues = getCurrentDisplay(reels);
    for(int i = 0; i < NUM_REELS; i++){
        printf(" %d |", currentValues[i]);
    }
    printf("\n");
}

//return current display(values) like {1, 2, 3}
int *getCurrentDisplay(Reel *reels[]){
    int currentDisplay[NUM_REELS];
    for(int i = 0; i < NUM_REELS; i++){
        currentDisplay[i] = getCurrentReelValue(reels[i]);
    }
    int *vp = &currentDisplay[0];
    return vp;
}

//return current value of reel that is specified as an argument
int getCurrentReelValue(Reel *reel){
    return reel->reel[reel->currentPoint];
}

void validate(){
    assert(NUM_REELS > 1);
    assert(REELLEN > 0);
}

int showResult(Reel *reels[]){
    clearConsole();
    showCurrentDisplay(reels);
    //check all value is same
    for(int i = 1; i < NUM_REELS; i++){
        if(reels[i]->reel[0] != reels[i-1]->reel[0]){
            printf("lose\n");
            return -1;
        }
    }
    //if pass all checks
    printf("win\n");
    return 0;
}

void* asyncKeyMonitor(void *structp){
    //void => SlotMachine
    SlotMachine *slotMachine = (SlotMachine *)structp;
    int pushedValues[NUM_REELS];

    int i = 0;
    for(;;){
        //if all input done, exit thread
        if(i == NUM_REELS){
            slotMachine->isAllPushed = 1;
            pthread_exit(NULL);
        }

        int temp = getchar();
        if(temp == ASCII_ENTER){
            pushedValues[i] = getCurrentReelValue(slotMachine->reels[i]);
            fixAllReelValue(slotMachine->reels[i], pushedValues[i]);
            i++;
        }
    }
}

void* asyncReelsRotator(void *structp){
    SlotMachine *slotMachine = (SlotMachine *)structp;
    for(;;){
        clearConsole();
        // if all input done, exit thread
        if(slotMachine->isAllPushed != 0){
            pthread_exit(NULL);
        }
        goAheadReels(slotMachine->reels);
        showCurrentDisplay(slotMachine->reels);
        usleep(SLEEP_MILSEC);
    }
}

//value ==> reel->reel[]
void fixAllReelValue(Reel *reel, int value){
    for(int i = 0; i < REELLEN; i++){
        reel->reel[i] = value;
    }
}