/*
 * auth.c - admin accounts: register, login, add new admin,
 *          forgot username and forgot password.
 *
 * How recovery works: when an admin is created, he/she picks a
 * security question and gives an answer. The answer is used to
 * recover the username or to reset the password.
 *
 * Passwords and answers are stored as a hash number (hashText),
 * not as readable text.
 */
#include "expense.h"

#define NUM_QUESTIONS 3
#define INPUT_SIZE    64                       /* big buffer for typed text */

static const char *securityQuestions[NUM_QUESTIONS] = {
    "What is the name of your first school?",
    "What is your favourite colour?",
    "What is your pet's name?"
};

/* hashText: turns text into a number (djb2 hash method).
 * The same text always gives the same number. */
static unsigned long hashText(const char *text) {
    unsigned long hash = 5381;
    while (*text) {
        hash = hash * 33 + (unsigned char)*text;
        text++;
    }
    return hash;
}

/* toLowerText: changes text to small letters (so answers are not
 * case-sensitive: "Blue" and "blue" are the same). */
static void toLowerText(char *text) {
    while (*text) {
        *text = (char)tolower((unsigned char)*text);
        text++;
    }
}

/* isValidUsername: 3 to 19 characters, only letters, digits or '_'. */
static int isValidUsername(const char *name) {
    int i, len = (int)strlen(name);
    if (len < 3 || len >= USER_LEN) {
        return 0;
    }
    for (i = 0; i < len; i++) {
        if (!isalnum((unsigned char)name[i]) && name[i] != '_') {
            return 0;
        }
    }
    return 1;
}

/* isValidPassword: 4 to 19 characters, no spaces. */
static int isValidPassword(const char *pass) {
    int i, len = (int)strlen(pass);
    if (len < 4 || len >= PASS_LEN) {
        return 0;
    }
    for (i = 0; i < len; i++) {
        if (isspace((unsigned char)pass[i])) {
            return 0;
        }
    }
    return 1;
}

/* findUserIndex: returns position of the username, or -1 if not found. */
static int findUserIndex(const char *name) {
    int i;
    for (i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* chooseQuestion: shows the security questions, returns 1 to 3. */
static int chooseQuestion(void) {
    int i, choice;
    printf("Security questions:\n");
    for (i = 0; i < NUM_QUESTIONS; i++) {
        printf("  %d. %s\n", i + 1, securityQuestions[i]);
    }
    while (1) {
        choice = readInt("Choose question number: ");
        if (choice >= 1 && choice <= NUM_QUESTIONS) {
            return choice;
        }
        printf("  Please choose between 1 and %d.\n", NUM_QUESTIONS);
    }
}

/* readNewPassword: asks for a password twice, until both match and the
 * password is valid. The result is stored in 'password'. */
static void readNewPassword(char *password) {
    char confirm[INPUT_SIZE];
    while (1) {
        readText("Enter password (4-19 characters, no spaces): ",
                 password, INPUT_SIZE);
        if (!isValidPassword(password)) {
            printf("  Invalid password. Use 4 to 19 characters, no spaces.\n");
            continue;
        }
        readText("Confirm password: ", confirm, sizeof(confirm));
        if (strcmp(password, confirm) == 0) {
            return;
        }
        printf("  Passwords do not match. Try again.\n");
    }
}

/*
 * createAdmin: asks all details and saves a new admin account.
 * Used by both "Register Admin" and "Add New Admin".
 * Returns 1 if created, 0 if not.
 */
static int createAdmin(void) {
    struct User u;
    char name[INPUT_SIZE], pass[INPUT_SIZE], answer[INPUT_SIZE];

    if (userCount >= MAX_USERS) {
        printf("Maximum number of admins (%d) reached.\n", MAX_USERS);
        return 0;
    }

    /* 1. username (valid and not already used) */
    while (1) {
        readText("Enter new username (3-19 letters, digits or _): ",
                 name, sizeof(name));
        if (!isValidUsername(name)) {
            printf("  Invalid username.\n");
        } else if (findUserIndex(name) >= 0) {
            printf("  This username is already taken.\n");
        } else {
            break;
        }
    }

    /* 2. password */
    readNewPassword(pass);

    /* 3. security question and answer (for Forgot Username/Password) */
    printf("Choose a security question for account recovery.\n");
    u.questionNo = chooseQuestion();
    do {
        readText("Enter your answer: ", answer, sizeof(answer));
        if (isBlank(answer)) {
            printf("  Answer cannot be empty.\n");
        }
    } while (isBlank(answer));
    toLowerText(answer);

    /* 4. fill the struct and save */
    strcpy(u.username, name);                  /* safe: length checked      */
    u.passHash = hashText(pass);
    u.answerHash = hashText(answer);
    users[userCount] = u;
    userCount++;
    saveUsers();
    return 1;
}

/*
 * registerAdmin: first-time setup. Allowed ONLY when no admin exists.
 * After that, more admins are added from inside using "Add New Admin".
 */
static void registerAdmin(void) {
    printf("\n--- Register Admin ---\n");
    if (userCount > 0) {
        printf("An admin is already registered.\n");
        printf("Please login and use 'Add New Admin' to create more admins.\n");
        return;
    }
    if (createAdmin()) {
        printf("Admin registered successfully. You can now login.\n");
    }
}

/* addNewAdmin: a logged-in admin creates another admin account. */
void addNewAdmin(void) {
    printf("\n--- Add New Admin ---\n");
    printf("Logged in as: %s\n", currentUser);
    if (createAdmin()) {
        printf("New admin added successfully.\n");
    }
}

/*
 * loginUser: asks username and password (3 attempts).
 * Returns 1 on success and remembers the name in currentUser.
 */
static int loginUser(void) {
    char name[INPUT_SIZE], pass[INPUT_SIZE];
    int attempt, index;

    printf("\n--- Login ---\n");
    for (attempt = 1; attempt <= 3; attempt++) {
        readText("Username: ", name, sizeof(name));
        readText("Password: ", pass, sizeof(pass));
        index = findUserIndex(name);
        if (index >= 0 && users[index].passHash == hashText(pass)) {
            strcpy(currentUser, users[index].username);
            printf("Login successful. Welcome, %s!\n", currentUser);
            return 1;
        }
        printf("Wrong username or password. Attempts left: %d\n", 3 - attempt);
    }
    printf("Too many wrong attempts.\n");
    return 0;
}

/* forgotUsername: shows the username if the security answer matches. */
static void forgotUsername(void) {
    char answer[INPUT_SIZE];
    int q, i, found = 0;

    printf("\n--- Forgot Username ---\n");
    if (userCount == 0) {
        printf("No admin is registered yet.\n");
        return;
    }
    q = chooseQuestion();
    readText("Enter your answer: ", answer, sizeof(answer));
    toLowerText(answer);
    for (i = 0; i < userCount; i++) {
        if (users[i].questionNo == q && users[i].answerHash == hashText(answer)) {
            printf("Your username is: %s\n", users[i].username);
            found = 1;
        }
    }
    if (!found) {
        printf("No admin matches this question and answer.\n");
    }
}

/* forgotPassword: lets the admin set a new password if the security
 * answer is correct. */
static void forgotPassword(void) {
    char name[INPUT_SIZE], answer[INPUT_SIZE], newPass[INPUT_SIZE];
    int index;

    printf("\n--- Forgot Password ---\n");
    readText("Enter your username: ", name, sizeof(name));
    index = findUserIndex(name);
    if (index < 0) {
        printf("Username not found.\n");
        return;
    }
    printf("Security question: %s\n",
           securityQuestions[users[index].questionNo - 1]);
    readText("Enter your answer: ", answer, sizeof(answer));
    toLowerText(answer);
    if (users[index].answerHash != hashText(answer)) {
        printf("Wrong answer. Password not changed.\n");
        return;
    }
    readNewPassword(newPass);
    users[index].passHash = hashText(newPass);
    saveUsers();
    printf("Password changed successfully. You can now login.\n");
}

/*
 * loginMenu: the first screen of the program.
 * Returns 1 when an admin has logged in, 0 if the user chose Exit.
 */
int loginMenu(void) {
    int choice;

    while (1) {
        printf("\n=========================================\n");
        printf("        PERSONAL EXPENSE TRACKER\n");
        printf("=========================================\n");
        printf("  1. Login\n");
        printf("  2. Register Admin (first-time setup)\n");
        printf("  3. Forgot Username\n");
        printf("  4. Forgot Password\n");
        printf("  5. Exit\n");
        printf("-----------------------------------------\n");
        if (userCount == 0) {
            printf("No admin found. Choose 2 to register the first admin.\n");
        }
        choice = readInt("Enter your choice (1-5): ");

        switch (choice) {
            case 1:
                if (userCount == 0) {
                    printf("No admin registered yet. Please choose 2 first.\n");
                } else if (loginUser()) {
                    return 1;                  /* success: go to main menu  */
                }
                break;
            case 2: registerAdmin();  break;
            case 3: forgotUsername(); break;
            case 4: forgotPassword(); break;
            case 5: return 0;                  /* exit program              */
            default: printf("Invalid choice! Please enter 1 to 5.\n");
        }
        pauseScreen();
    }
}
