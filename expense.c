/* expense.c - the main record operations: add, view, search, update, delete */
#include "expense.h"

/* =========================================================
 *                    SMALL HELPERS
 * ========================================================= */

/* findIndexById: returns array position of the record with this ID,
 * or -1 if no such record exists. */
int findIndexById(int id) {
    int i;
    for (i = 0; i < expenseCount; i++) {
        if (expenses[i].id == id) {
            return i;
        }
    }
    return -1;
}

/* printHeader: prints the table heading line. */
void printHeader(void) {
    printf("\n%-5s %-12s %-16s %-12s %s\n",
           "ID", "Date", "Category", "Amount(Rs)", "Description");
    printf("---------------------------------------------------------------------\n");
}

/* printRow: prints one expense as one table row. */
void printRow(const struct Expense *e) {
    printf("%-5d %-12s %-16s %-12.2f %s\n",
           e->id, e->date, e->category, e->amount, e->description);
}

/* =========================================================
 *                    MAIN FEATURES
 * ========================================================= */

/*
 * addExpense: takes details from the user and stores a new record.
 */
void addExpense(void) {
    struct Expense e;                          /* temporary record          */

    if (expenseCount >= MAX_EXPENSES) {        /* array is full             */
        printf("Storage full! Cannot add more expenses.\n");
        return;
    }

    printf("\n--- Add New Expense ---\n");
    e.id = nextId;                             /* give next free ID         */
    readDate(e.date);                          /* validated date            */
    chooseCategory(e.category);                /* choose from list          */
    e.amount = readAmount("Enter amount (Rs): "); /* must be positive       */

    /* description must not be empty */
    do {
        printf("Enter description (max %d chars): ", DESC_LEN - 1);
        readLine(e.description, DESC_LEN);
        if (e.description[0] == '\0') {
            printf("  Description cannot be empty.\n");
        }
    } while (e.description[0] == '\0');

    expenses[expenseCount] = e;                /* store in the array        */
    expenseCount++;
    nextId++;
    saveData();                                /* write to file             */

    printf("Expense added successfully with ID %d.\n", e.id);
    checkBudget();                             /* bonus: budget alert       */
}

/*
 * viewExpenses: prints all records in a table, plus the total.
 */
void viewExpenses(void) {
    int i;
    float total = 0;

    printf("\n--- All Expenses ---\n");
    if (expenseCount == 0) {                   /* handle empty list         */
        printf("No expenses recorded yet.\n");
        return;
    }
    printHeader();
    for (i = 0; i < expenseCount; i++) {
        printRow(&expenses[i]);
        total += expenses[i].amount;
    }
    printf("---------------------------------------------------------------------\n");
    printf("Total records: %d      Total spent: Rs %.2f\n", expenseCount, total);
}

/*
 * searchExpense: finds records by ID, Date or Category.
 */
void searchExpense(void) {
    int choice, i, id, found = 0;
    char text[DATE_LEN];
    char category[CAT_LEN];

    printf("\n--- Search Expense ---\n");
    printf("  1. Search by ID\n  2. Search by Date\n  3. Search by Category\n");
    choice = readInt("Enter choice: ");

    if (choice == 1) {                         /* ----- search by ID ----- */
        id = readInt("Enter expense ID: ");
        i = findIndexById(id);
        if (i >= 0) {
            printHeader();
            printRow(&expenses[i]);
            found = 1;
        }
    } else if (choice == 2) {                  /* ---- search by date ---- */
        readDate(text);
        for (i = 0; i < expenseCount; i++) {
            if (strcmp(expenses[i].date, text) == 0) {
                if (!found) printHeader();     /* print heading only once   */
                printRow(&expenses[i]);
                found = 1;
            }
        }
    } else if (choice == 3) {                  /* --- search by category -- */
        chooseCategory(category);
        for (i = 0; i < expenseCount; i++) {
            if (strcmp(expenses[i].category, category) == 0) {
                if (!found) printHeader();
                printRow(&expenses[i]);
                found = 1;
            }
        }
    } else {
        printf("Invalid choice.\n");
        return;
    }

    if (!found) {
        printf("No matching record found.\n"); /* graceful "not found"      */
    }
}

/*
 * updateExpense: changes the amount, category or description
 * of an existing record (found by ID).
 */
void updateExpense(void) {
    int id, index, choice;

    printf("\n--- Update Expense ---\n");
    if (expenseCount == 0) {
        printf("No expenses to update.\n");
        return;
    }
    id = readInt("Enter ID of expense to update: ");
    index = findIndexById(id);
    if (index < 0) {
        printf("No record found with ID %d.\n", id);
        return;
    }

    printHeader();
    printRow(&expenses[index]);                /* show current values       */

    printf("\nWhat do you want to change?\n");
    printf("  1. Amount\n  2. Category\n  3. Description\n");
    choice = readInt("Enter choice: ");

    if (choice == 1) {
        expenses[index].amount = readAmount("Enter new amount (Rs): ");
    } else if (choice == 2) {
        chooseCategory(expenses[index].category);
    } else if (choice == 3) {
        printf("Enter new description: ");
        readLine(expenses[index].description, DESC_LEN);
        if (expenses[index].description[0] == '\0') {
            strcpy(expenses[index].description, "N/A");
        }
    } else {
        printf("Invalid choice. Nothing changed.\n");
        return;
    }

    saveData();
    printf("Expense updated successfully.\n");
    checkBudget();                             /* amount may have changed   */
}

/*
 * deleteExpense: removes a record by ID after asking for confirmation.
 * Records after it are shifted one place left to fill the gap.
 */
void deleteExpense(void) {
    int id, index, i;
    char answer[8];

    printf("\n--- Delete Expense ---\n");
    if (expenseCount == 0) {
        printf("No expenses to delete.\n");
        return;
    }
    id = readInt("Enter ID of expense to delete: ");
    index = findIndexById(id);
    if (index < 0) {
        printf("No record found with ID %d.\n", id);
        return;
    }

    printHeader();
    printRow(&expenses[index]);
    printf("Are you sure you want to delete this record? (y/n): ");
    readLine(answer, sizeof(answer));

    if (answer[0] == 'y' || answer[0] == 'Y') {
        for (i = index; i < expenseCount - 1; i++) {
            expenses[i] = expenses[i + 1];     /* shift records left        */
        }
        expenseCount--;
        saveData();
        printf("Expense deleted successfully.\n");
    } else {
        printf("Delete cancelled.\n");
    }
}
