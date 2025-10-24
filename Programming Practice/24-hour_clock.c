#include <stdio.h>

int main(){
    int hours;
    int minutes;

    for(hours = 0; hours < 24; hours++){

        for (minutes = 0; minutes < 60; minutes++){
                printf("%d:%02d\n", hours, minutes);

        } // end inner for
        
    } // end outer for

    return 0;
} // end main()