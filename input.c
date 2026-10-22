/* input.c - all functions that read and validate keyboard input */
#include "expense.h"

/* =========================================================
 *                    INPUT HELPER FUNCTIONS
 * ========================================================= */

/*
 * readLine: safely reads one line of text from the keyboard.
 * - fgets never writes more than 'size' characters, so no buffer overflow.
 * - Removes the trailing newline '\n'.
 * - If the user typed more than the buffer can hold, the extra
 *   characters are thrown away so they don't spoil the next input.
 * Returns 1 on success, 0 if input ended (e.g. Ctrl+D).
 */
int readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0;                              /* no input available        */
    }
    int len = (int)strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';                /* remove the newline        */
    } else {
        int ch;                                /* line was too long:        */
        while ((ch = getchar()) != '\n' && ch != EOF) {
            ;                                  /* discard the leftover      */
        }
    }
    return 1;
}

/*
 * readInt: shows a prompt and keeps asking until the user types
 * a valid whole number. Used for menu choices and IDs.
 */
int readInt(const char *prompt) {
    char buffer[32];
    char *end;
    long value;

    while (1) {
        printf("%s", prompt);
        if (!readLine(buffer, sizeof(buffer))) {
            printf("\nInput closed. Exiting.\n");
            saveData();
            exit(0);
        }
        value = strtol(buffer, &end, 10);      /* convert text to number    */
        /* valid only if something was read and nothing else is left over  */
        if (end != buffer && *end == '\0') {
            return (int)value;
        }
        printf("  Invalid input. Please enter a whole number.\n");
    }
}

/*
 * readAmount: asks for a money amount. Rejects letters, zero and
 * negative numbers (strict validation).
 */
float readAmount(const char *prompt) {
    char buffer[32];
    char *end;
    double value;

    while (1) {
        printf("%s", prompt);
        if (!readLine(buffer, sizeof(buffer))) {
            saveData();
            exit(0);
        }
        value = strtod(buffer, &end);          /* convert text to decimal   */
        if (end == buffer || *end != '\0') {
            printf("  Invalid amount. Please enter a number like 250.50\n");
        } else if (value <= 0) {
            printf("  Amount must be greater than 0 (no negative values).\n");
        } else if (value > 10000000) {
            printf("  Amount too large. Please enter a realistic value.\n");
        } else {
            return (float)value;
        }
    }
}

/*
 * isValidDate: checks that text looks like DD-MM-YYYY and that
 * day, month and year are in sensible ranges.
 * Returns 1 if valid, 0 if not.
 */
int isValidDate(const char *date) {
    int d, m, y;
    int daysInMonth[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (strlen(date) != 10 || date[2] != '-' || date[5] != '-') {
        return 0;                              /* wrong shape               */
    }
    if (sscanf(date, "%d-%d-%d", &d, &m, &y) != 3) {
        return 0;                              /* could not read 3 numbers  */
    }
    if (y < 2000 || y > 2100 || m < 1 || m > 12) {
        return 0;
    }
    if (d < 1 || d > daysInMonth[m - 1]) {     /* Feb allows up to 29 here  */
        return 0;
    }
    return 1;
}

/*
 * getTodayDate: fills 'date' with today's date as DD-MM-YYYY
 * using the system clock (<time.h>).
 */
void getTodayDate(char *date) {
    time_t now = time(NULL);                   /* seconds since 1970        */
    struct tm *t = localtime(&now);            /* break into day/month/year */
    sprintf(date, "%02d-%02d-%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
}

/*
 * readDate: asks for a date. Pressing Enter uses today's date.
 * Keeps asking until a valid date is typed.
 */
void readDate(char *date) {
    char buffer[32];

    while (1) {
        printf("Enter date (DD-MM-YYYY) or press Enter for today: ");
        if (!readLine(buffer, sizeof(buffer))) {
            saveData();
            exit(0);
        }
        if (buffer[0] == '\0') {               /* user pressed Enter only   */
            getTodayDate(date);
            printf("  Using today's date: %s\n", date);
            return;
        }
        if (isValidDate(buffer)) {
            strcpy(date, buffer);              /* safe: length is exactly 10*/
            return;
        }
        printf("  Invalid date. Example of a correct date: 05-03-2026\n");
    }
}

/*
 * chooseCategory: shows the category list and lets the user pick one
 * by number. The chosen name is copied into 'category'.
 */
void chooseCategory(char *category) {
    int i, choice;

    printf("Choose a category:\n");
    for (i = 0; i < categoryCount; i++) {
        printf("  %d. %s\n", i + 1, categories[i]);
    }
    while (1) {
        choice = readInt("Enter category number: ");
        if (choice >= 1 && choice <= categoryCount) {
            strcpy(category, categories[choice - 1]);
            return;
        }
        printf("  Please choose between 1 and %d.\n", categoryCount);
    }
}

/*
 * pauseScreen: waits until the user presses Enter.
 * Used after each feature so the output stays on the screen
 * and the menu does not appear immediately.
 */
void pauseScreen(void) {
    char dummy[8];
    printf("\nPress Enter to continue...");
    readLine(dummy, sizeof(dummy));            /* wait for the Enter key    */
}

/*
 * readText: shows a prompt and reads one line into 'buffer'.
 * If the input is closed (Ctrl+Z / Ctrl+D) the program saves and exits,
 * so we never loop forever.
 */
void readText(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    if (!readLine(buffer, size)) {
        printf("\nInput closed. Exiting.\n");
        saveData();
        exit(0);
    }
}

/* isBlank: returns 1 if the text is empty or only spaces. */
int isBlank(const char *text) {
    while (*text) {
        if (!isspace((unsigned char)*text)) {
            return 0;
        }
        text++;
    }
    return 1;
}

/* confirmAction: asks a yes/no question. Returns 1 for y/Y, else 0. */
int confirmAction(const char *prompt) {
    char answer[8];
    readText(prompt, answer, sizeof(answer));
    return (answer[0] == 'y' || answer[0] == 'Y');
}
