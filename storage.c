/* storage.c - saving to and loading from the data file (expenses.dat) */
#include "expense.h"

/* =========================================================
 *                    FILE HANDLING
 * ========================================================= */

/*
 * loadData: reads saved data from expenses.dat when the program starts.
 * File layout (binary):  [budget][count][nextId][record][record]...
 * If the file does not exist (first run) we simply start empty.
 */
void loadData(void) {
    FILE *fp = fopen(DATA_FILE, "rb");         /* rb = read binary          */
    if (fp == NULL) {
        return;                                /* first run: nothing to load*/
    }
    /* read the three header values; if any read fails, start fresh */
    if (fread(&monthlyBudget, sizeof(float), 1, fp) != 1 ||
        fread(&expenseCount, sizeof(int), 1, fp) != 1 ||
        fread(&nextId, sizeof(int), 1, fp) != 1 ||
        expenseCount < 0 || expenseCount > MAX_EXPENSES) {
        monthlyBudget = 0; expenseCount = 0; nextId = 1;
        fclose(fp);
        return;
    }
    /* read all records in one go */
    if (fread(expenses, sizeof(struct Expense), expenseCount, fp)
        != (size_t)expenseCount) {
        printf("Warning: data file is damaged. Starting with empty data.\n");
        expenseCount = 0; nextId = 1;
    }
    fclose(fp);
}

/*
 * saveData: writes budget, count and all records to expenses.dat.
 * Called after every change so nothing is lost.
 */
void saveData(void) {
    FILE *fp = fopen(DATA_FILE, "wb");         /* wb = write binary         */
    if (fp == NULL) {
        printf("Error: could not open %s for saving!\n", DATA_FILE);
        return;
    }
    fwrite(&monthlyBudget, sizeof(float), 1, fp);
    fwrite(&expenseCount, sizeof(int), 1, fp);
    fwrite(&nextId, sizeof(int), 1, fp);
    fwrite(expenses, sizeof(struct Expense), expenseCount, fp);
    fclose(fp);
}


/* loadUsers: reads admin accounts from users.dat (if the file exists). */
void loadUsers(void) {
    FILE *fp = fopen(USER_FILE, "rb");
    if (fp == NULL) {
        return;                                /* first run: no admins yet  */
    }
    if (fread(&userCount, sizeof(int), 1, fp) != 1 ||
        userCount < 0 || userCount > MAX_USERS ||
        fread(users, sizeof(struct User), userCount, fp) != (size_t)userCount) {
        userCount = 0;                         /* damaged file: start empty */
    }
    fclose(fp);
}

/* saveUsers: writes all admin accounts to users.dat. */
void saveUsers(void) {
    FILE *fp = fopen(USER_FILE, "wb");
    if (fp == NULL) {
        printf("Error: could not save %s!\n", USER_FILE);
        return;
    }
    fwrite(&userCount, sizeof(int), 1, fp);
    fwrite(users, sizeof(struct User), userCount, fp);
    fclose(fp);
}

/* loadCategories: reads the category list. If the file is missing or
 * damaged, the 5 default categories are used instead. */
void loadCategories(void) {
    FILE *fp = fopen(CAT_FILE, "rb");
    if (fp != NULL) {
        if (fread(&categoryCount, sizeof(int), 1, fp) == 1 &&
            categoryCount >= 1 && categoryCount <= MAX_CATEGORIES &&
            fread(categories, CAT_LEN, categoryCount, fp) == (size_t)categoryCount) {
            fclose(fp);
            return;                            /* loaded successfully       */
        }
        fclose(fp);
    }
    initDefaultCategories();                   /* fall back to defaults     */
    saveCategories();
}

/* saveCategories: writes the category list to categories.dat. */
void saveCategories(void) {
    FILE *fp = fopen(CAT_FILE, "wb");
    if (fp == NULL) {
        printf("Error: could not save %s!\n", CAT_FILE);
        return;
    }
    fwrite(&categoryCount, sizeof(int), 1, fp);
    fwrite(categories, CAT_LEN, categoryCount, fp);
    fclose(fp);
}
