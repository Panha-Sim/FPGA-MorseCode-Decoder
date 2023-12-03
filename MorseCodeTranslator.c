#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>

#include "./address_map_arm.h"
#include "./Hardware.h"
#include "./MorseCodeLookupTable.c"


#define DEBOUNCE_TIME 100 
#define DOT_THRESHOLD 100
#define DASH_THRESHOLD 300

volatile int *KEY_ptr;

long getCurrTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void delayMilliseconds(int milliseconds) 
{
        usleep(milliseconds*1000);
}

void concatStr(char* dest, const char* src, size_t destSize) {
    if (strlen(dest) + strlen(src) < destSize) {
        strcat(dest, src);
    }
}

int main(void){
    
    int fd = -1;
    init(fd);

    KEY_ptr = readKey();  
    int prev_key_value = *KEY_ptr;
    int buttonValue = *KEY_ptr;

    int pressTime;
    int releaseTime;
    char *result;
    
    char codeArray[10] = "";
    char sentences[10000] = "";
    
    while(1) {
        
        buttonValue = *KEY_ptr;
        
        // State Change 
        if(buttonValue!= prev_key_value) {


            if(buttonValue == 4) {
                printf("%s\n","Exiting Program...");
                return 0;
            }

            //delayMilliseconds(DEBOUNCE_TIME);
            if(buttonValue == 2) {
                result = lookup(codeArray);
                char resultStr[2] = {result, '\0'}; // Convert char to string
                concatStr(sentences, resultStr, sizeof(sentences)); // Concatenate string
                printf("%s\n", sentences);
                memset(codeArray, '\0', sizeof(codeArray));
            }

            if(buttonValue == 1) {
                
                // Get initial Time
                pressTime = getCurrTime();

            }else if(buttonValue == 0 && prev_key_value != 2) {
                // Get release Time & Find its duration
                releaseTime = getCurrTime();
                int duration = releaseTime - pressTime;

                // Check if its dot or dash
                if(duration > DOT_THRESHOLD && duration < DASH_THRESHOLD) {
                    concatStr(codeArray, ".", sizeof(codeArray));
                }else if(duration >= DASH_THRESHOLD) {
                    concatStr(codeArray, "-", sizeof(codeArray));
                }
                printf("%s\n", codeArray);
            }

            
            // UpdatepPrevious button value
            prev_key_value = *KEY_ptr;
        }
        
    }  
    

    close_physical(fd);
    return 0;
}