//RUNS ONLY ON LINUX
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<assert.h>
#include<pthread.h>

#define REELLEN 10      // number of max reel value
#define NUM_REELS 3     // number of reels to be shown
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
}ReelsInfo;

Reel *ConstructReel(int);
ReelsInfo *ConstructReelsInfo(Reel *[]);
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
    //create reel, init by random value
    Reel *reels[NUM_REELS];
    srand((unsigned int)time(NULL));
    for(int i = 0; i < NUM_REELS; i++){
        reels[i] = ConstructReel(rand()%REELLEN);
    }

    ReelsInfo *reelsInfo = ConstructReelsInfo(reels);
    //rotate reels, await keyboard hits
    pthread_t awaitKeyboardHits;
    pthread_t showingThread;
    pthread_create(&awaitKeyboardHits, NULL, asyncKeyMonitor, reelsInfo);
    pthread_create(&showingThread, NULL, asyncReelsRotator, reelsInfo);
    pthread_join(awaitKeyboardHits, NULL);
    pthread_join(showingThread, NULL);

    return showResult(reels);
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
    return reel;
}

ReelsInfo *ConstructReelsInfo(Reel *reels[]){
    ReelsInfo *reelsInfo = malloc(sizeof(ReelsInfo));
    for(int i = 0; i < NUM_REELS; i++){
        reelsInfo->reels[i] = reels[i];
    }
    reelsInfo->isAllPushed = 0;
    return reelsInfo;
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
            printf("failed\n");
            return -1;
        }
    }
    //if pass all checks
    printf("won\n");
    return 0;
}

void* asyncKeyMonitor(void *structp){
    //void => Reelsinfo
    ReelsInfo *reelsInfo = (ReelsInfo *)structp;
    int pushedValues[NUM_REELS];

    int i = 0;
    for(;;){
        //if all input done, exit thread
        if(i == NUM_REELS){
            reelsInfo->isAllPushed = 1;
            return 0;
        }

        int temp = getchar();
        if(temp == ASCII_ENTER){
            pushedValues[i] = getCurrentReelValue(reelsInfo->reels[i]);
            fixAllReelValue(reelsInfo->reels[i], pushedValues[i]);
            i++;
        }
    }
}

void* asyncReelsRotator(void *structp){
    for(;;){
        ReelsInfo *reelsInfo = (ReelsInfo *)structp;
        clearConsole();
        // if all input done, exit thread
        if(reelsInfo->isAllPushed != 0){
            return 0;
        }
        goAheadReels(reelsInfo->reels);
        showCurrentDisplay(reelsInfo->reels);
        usleep(SLEEP_MILSEC);
    }
}

//value ==> reel->reel[]
void fixAllReelValue(Reel *reel, int value){
    for(int i = 0; i < REELLEN; i++){
        reel->reel[i] = value;
    }
}