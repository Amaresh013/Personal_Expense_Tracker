/*
 * main.c - program start point: global variables, menus and main loop
 * Course : MCA 1st Semester - Problem Solving Using C (MC-T-PC-102)
 * College: GIFT-Autonomous
 */
#include "expense.h"

/* ---------------- Global Variables (declared "extern" in expense.h) ---- */
struct Expense expenses[MAX_EXPENSES];  /* all expense records               */
int   expenseCount  = 0;                /* records in use                    */
int   nextId        = 1;                /* ID for the next expense           */
float monthlyBudget = 0.0f;             /* 0 means "budget not set"          */
struct User users[MAX_USERS];           /* all admin accounts                */
int   userCount     = 0;                /* admins in use                     */
char  currentUser[USER_LEN] = "";       /* name of logged-in admin           */
char  categories[MAX_CATEGORIES][CAT_LEN]; /* category names                 */
int   categoryCount = 0;                /* categories in use                 */

/* showMenu: prints the main menu (shown after login). */
static void showMenu(void) {
    printf("\n=========================================\n");
    printf("        PERSONAL EXPENSE TRACKER\n");
    printf("        Logged in as: %s\n", currentUser);
    printf("=========================================\n");
    printf("   1. Add New Expense\n");
    printf("   2. View All Expenses\n");
    printf("   3. Search Expense\n");
    printf("   4. Update Expense\n");
    printf("   5. Delete Expense\n");
    printf("   6. Expense Summary & Reports\n");
    printf("   7. Budget Settings (Set/Update/Delete)\n");
    printf("   8. Manage Categories (Add/Delete)\n");
    printf("   9. Add New Admin\n");
    printf("   0. Save & Logout\n");
    printf("-----------------------------------------\n");
}

/*
 * adminMenu: the main menu loop for a logged-in admin.
 * It runs until the admin chooses "Save & Logout", then control
 * goes back to the login screen (which has the Exit option).
 */
static void adminMenu(void) {
    int choice, needPause;

    while (1) {
        showMenu();
        choice = readInt("Enter your choice (0-9): ");
        needPause = 1;                         /* most options wait for Enter */

        switch (choice) {
            case 1:  addExpense();       break;
            case 2:  viewExpenses();     break;
            case 3:  searchExpense();    break;
            case 4:  updateExpense();    break;
            case 5:  deleteExpense();    break;
            case 6:  calculateSummary(); break;
            case 7:  budgetMenu();   needPause = 0; break;  /* has own pause */
            case 8:  categoryMenu(); needPause = 0; break;  /* has own pause */
            case 9:  addNewAdmin();      break;
            case 0:                                /* -- Save & Logout -- */
                saveData();                    /* expenses and budget       */
                saveUsers();                   /* admin accounts            */
                saveCategories();              /* category list             */
                printf("All data saved. Logged out. Goodbye, %s!\n", currentUser);
                currentUser[0] = '\0';         /* nobody is logged in now   */
                return;                        /* back to the login screen  */
            default:
                printf("Invalid choice! Please enter 0 to 9.\n");
        }
        if (needPause) {
            pauseScreen();                     /* wait before showing menu  */
        }
    }
}

int main(void) {
    /* 1. load everything saved from the last run */
    loadData();                                /* expenses and budget       */
    loadUsers();                               /* admin accounts            */
    loadCategories();                          /* category list             */

    /* 2. login screen -> main menu -> (logout) -> login screen ... */
    while (loginMenu()) {                      /* 0 means user chose Exit   */
        printf("%d saved expense record(s) loaded.\n", expenseCount);
        checkBudget();                         /* show budget status        */
        pauseScreen();
        adminMenu();                           /* returns after Save & Logout */
    }

    saveData();                                /* final save before exit    */
    return 0;
}
