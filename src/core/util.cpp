#include "util.h"
#include<iostream>
using namespace std;

// Primitive helper utilities, clamping, and screen control


// Energy and Time stay bw 0 and 100
int clampInt(int val, int minVal, int maxVal){
    if (val < minVal) return minVal;
    elif (val > maxVal) return maxVal;
    return val;
}

// Remove spaces and line endings from start and end of text
void trimWhitespace(char *str){
    int start = 0;
    while (str[start]) == ' ' || str[start] == '\t' || str[start] == '\r' || str[start] == '\n'{
        start += 1;
    }

    int end = start;
    while (str[end] != '\0') end +=1;
    while ((end > start) && (str[end - 1] == ' ') || (str[end - 1] == '\t') || (str[end-1] == '\r') || (str[end - 1] == '\n' )){
        end += 1;
    }
    int length = end - start;
    for (int i = 0; i < length; i++){
        str[i] = str[start+i];
    }
    str[length] = '\0';
}   

// compare 2 texts letter by letter
bool stringEquals(const char *a, const char *b){
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0'){
        if (a[i] != b[i]) return false;
        i += 1;
    }
    return a[i] == b[i];
}

//print title bw dash lines
void printHeader(const char *title){
    cout << "----------------------------------------\n";
    cout << " " << title << "\n";
    cout << "----------------------------------------\n";
}

// print black lines
void clearScreen(){
    for (int i = 0; i < 40; i++) cout << "\n";
}
