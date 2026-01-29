#include <stdio.h>
#include <string.h>

#define PASSWORD "supersecret"

char* password = PASSWORD;

int win() {
    printf("You win! yippeee!!!\n");
    return 0;
}

// Checks if a user supplied password matches a secret password
// Returns 0 if passwords match, -1 otherwise
int check_password() {
    int pass_match = 0;
    register int i = 0;
    char guess[12] = {0};

    for (i = 0; i < 64; i++) {
        // read in a character
        char c = getchar();
        guess[i] = c;
        if (c == '\n')
            break;
    }

    // compare if two passwords match
    for (i = 0; i < strlen(password); i++) {
        if (password[i] != guess[i]) {
            // passwords don't match
            pass_match = -1;
        }
    }

    return pass_match;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    
    printf("Enter the password: \n");
    if (check_password() == 0) {
        // you can't win >:)
        // return win();
    }

    printf("Not the right password :crying_face:\n");
    return 0;
}