// ============================================================================
// Copyright (c) 2013 by Terasic Technologies Inc.
// ============================================================================
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// ============================================================================
//           
//  Terasic Technologies Inc
//  9F., No.176, Sec.2, Gongdao 5th Rd, East Dist, Hsinchu City, 30070. Taiwan
//  
//  
//                     web: http://www.terasic.com/  
//                     email: support@terasic.com
//
// ============================================================================

#include "terasic_os_includes.h"
#include "LCD_Lib.h"
#include "lcd_graphic.h"
#include "font.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <math.h>


#include "./address_map_arm.h"
//#include "./Hardware.h"
#include "./MorseCodeLookupTable.c"


#define DEBOUNCE_TIME 100 
#define DOT_THRESHOLD 100
#define DASH_THRESHOLD 300

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

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

int space = 40;
int height = 5;
void writeCLD(char *word, LCD_CANVAS LcdCanvas, void *virtual_base){
	space += 10;
	if(*word==';'){
		space = 40;
		height += 13;
		return;
	}

    DRAW_PrintString(&LcdCanvas, space, height, word, LCD_BLACK, &font_16x16);
    DRAW_Refresh(&LcdCanvas);	

}

int main() {

	void *LW_virtual;
	void *virtual_base;
	int fd;

		
	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	if ((LW_virtual = map_physical (fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)) == NULL){
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	
	if ((virtual_base = map_physical (fd, HW_REGS_BASE, HW_REGS_SPAN)) == NULL){
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	// int fd = -1;
	// init(fd);


	LCD_CANVAS LcdCanvas;

	LcdCanvas.Width = LCD_WIDTH;
	LcdCanvas.Height = LCD_HEIGHT;
	LcdCanvas.BitPerPixel = 1;
	LcdCanvas.FrameSize = LcdCanvas.Width * LcdCanvas.Height / 8;
	LcdCanvas.pFrame = (void *)malloc(LcdCanvas.FrameSize);
		
	if (LcdCanvas.pFrame == NULL){
			printf("failed to allocate lcd frame buffer\r\n");
	}else{			
		
	
		LCDHW_Init(virtual_base);
		LCDHW_BackLight(true); // turn on LCD backlight
		
		LCD_Init();
	
		// clear screen
		DRAW_Clear(&LcdCanvas, LCD_WHITE);

			// demo grphic api    
		DRAW_Rect(&LcdCanvas, 0,0, LcdCanvas.Width-1, LcdCanvas.Height-1, LCD_BLACK); // retangle
		DRAW_Circle(&LcdCanvas, 10, 10, 6, LCD_BLACK);
		DRAW_Circle(&LcdCanvas, LcdCanvas.Width-10, 10, 6, LCD_BLACK);
		DRAW_Circle(&LcdCanvas, LcdCanvas.Width-10, LcdCanvas.Height-10, 6, LCD_BLACK);
		DRAW_Circle(&LcdCanvas, 10, LcdCanvas.Height-10, 6, LCD_BLACK);
	}    


	volatile int *KEY_ptr = (unsigned int*)(LW_virtual + KEY_BASE);
	
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
				writeCLD(&result, LcdCanvas, virtual_base);
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

   free(LcdCanvas.pFrame);
	// clean up our memory mapping and exit
	
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

   
	close( fd );

	return( 0 );
}
