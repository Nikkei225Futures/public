#include<stdio.h>
#include<string.h>

#define MAXLEN 1024

void removeTailSpaces(char *str);

int main(){
    int i = 0x12345678;
    unsigned char *p = (unsigned char *)&i;
    FILE *fp;
    char *cpuName;

    // open cpuinfo at /proc/cpuinfo
    if((fp = fopen("/proc/cpuinfo", "r")) == NULL){
        printf("can not open cpuinfo\n");
        cpuName = "your cpu";
    }
    cpuName = "your cpu";
    
    char singleRow[MAXLEN];
    // read each row
    while(fgets(singleRow, MAXLEN, fp) != NULL){
        char *headAddr = strstr(singleRow, "model name");
        
        if(headAddr != NULL){       // if "model name" included in singleRow
            int i = 0;
            int j = 0;
            int isAfterCoron = 1;   //use like boolean flag, 1 = false, 0 = true
            char strAfterCoron[MAXLEN];
            
            for(;;i++){

                // if isAfterCoron == true, copy character to strAfterCoron
                if(isAfterCoron == 0){
                    strAfterCoron[j] = singleRow[i];
                    j++;
                }

                // if ':' appear in a row, true -> isAfterCoron
                if(singleRow[i] == ':'){
                    isAfterCoron = 0;
                }else if(singleRow[i] == '\n'){
                    //if the coron found(= cpuName exists) && index reached to tail of str.
                    if(isAfterCoron == 0){           
                        cpuName = strAfterCoron;
                    }
                    break;      //stop searching
                }

            }
            break;  //stop getting row, if there is cpu name("model name");
        }
    }
    fclose(fp);
    removeTailSpaces(cpuName);


    if(p[0] == 0x78){
        printf("%s is little endien\n", cpuName);
    }else{
        printf("%s is big endien\n", cpuName);
    }
    return 0;
}

// remove unnecessary spaces at tail
//(add '\0' from tail side until displayable letter appears)
void removeTailSpaces(char* str){
    int strlength = strlen(str);
    for(int i = strlength-1; str[i] <= 32; i--){
        str[i] = '\0';
    }
}