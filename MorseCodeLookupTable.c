#include <stdio.h>
#include <unistd.h>

typedef struct {
    char* morseCode;
    char character;
}MorseCode;

static const MorseCode morseTable[] = {
    {".-", 'A'},
    {"-...", 'B'},
    {"-.-.", 'C'},
    {"-..", 'D'},
    {".", 'E'},
    {"..-.", 'F'},
    {"--.", 'G'},
    {"....", 'H'},
    {"..", 'I'},
    {".---", 'J'},
    {"-.-", 'K'},
    {".-..", 'L'},
    {"--", 'M'},
    {"-.", 'N'},
    {"---", 'O'},
    {".--.", 'P'},
    {"--.-", 'Q'},
    {".-.", 'R'},
    {"...", 'S'},
    {"-", 'T'},
    {"..-", 'U'},
    {"...-", 'V'},
    {".--", 'W'},
    {"-..-", 'X'},
    {"-.--", 'Y'},
    {"--..", 'Z'},
    {"-----", '0'},
    {".----", '1'},
    {"..---", '2'},
    {"...--", '3'},
    {"....-", '4'},
    {".....", '5'},
    {"-....", '6'},
    {"--...", '7'},
    {"---..", '8'},
    {"----.", '9'},
    {".-.-.-", '.'},  // Period
    {"--..--", ','},  // Comma
    {"..--..", '?'},  // Question Mark
    {"-.-.--", '!'},  // Exclamation mark
    {"-....-", '-'},  // Dash or Hyphen
    {"-..-.", '/'},   // Slash or Division
    {".--.-.", '@'},  // At symbol
    {"-.--.", '('},   // Left Parenthesis
    {"-.--.-", ')'},  // Right Parenthesis
    {"", '\0'}       // End of table marker
};

static char lookup(char* inputCode){
    //int size = sizeof(morseTable) / sizeof(morseTable[0]);
    for(int i=0; i<45; i++){
        if(strcmp(morseTable[i].morseCode, inputCode)==0){
            return morseTable[i].character;
        }
    }
    return ';';
    
}