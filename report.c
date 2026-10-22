/* report.c - summary report and monthly budget alert (bonus feature) */
#include "expense.h"

/*
 * calculateSummary: shows total spending and spending per category.
 */
void calculateSummary(void) {
    int i, c;
    float total = 0;
    float categoryTotal[MAX_CATEGORIES] = {0}; /* one total per category    */

    printf("\n--- Expense Summary & Report ---\n");
    if (expenseCount == 0) {
        printf("No expenses recorded yet.\n");
        return;
    }

    /* go through every record and add its amount to the right totals */
    for (i = 0; i < expenseCount; i++) {
        total += expenses[i].amount;
        for (c = 0; c < categoryCount; c++) {
            if (strcmp(expenses[i].category, categories[c]) == 0) {
                categoryTotal[c] += expenses[i].amount;
            }
        }
    }

    printf("Total spending : Rs %.2f\n\n", total);
    printf("%-16s %-14s %s\n", "Category", "Amount(Rs)", "Share");
    printf("------------------------------------------\n");
    for (c = 0; c < categoryCount; c++) {
        printf("%-16s %-14.2f %.1f%%\n", categories[c], categoryTotal[c],
               (categoryTotal[c] / total) * 100);
    }
    checkBudget();                             /* show budget status too    */
}

/*
 * setBudget: lets the user set the monthly budget limit.
 */
void setBudget(void) {
    printf("\n--- Set / Update Monthly Budget ---\n");
    if (monthlyBudget > 0) {
        printf("Current budget: Rs %.2f\n", monthlyBudget);
    }
    monthlyBudget = readAmount("Enter new monthly budget (Rs): ");
    saveData();
    printf("Budget saved.\n");
    checkBudget();
}

/*
 * checkBudget (BONUS FEATURE): adds up the spending of the CURRENT
 * month and warns if it crosses the budget.
 * Month is matched using the "MM-YYYY" part of the date (position 3 on).
 */
void checkBudget(void) {
    char today[DATE_LEN];
    int i;
    float monthTotal = 0;

    if (monthlyBudget <= 0) {
        return;                                /* budget not set: skip      */
    }
    getTodayDate(today);                       /* e.g. 20-10-2026           */

    for (i = 0; i < expenseCount; i++) {
        /* compare only "MM-YYYY" (skip the first 3 chars "DD-") */
        if (strcmp(expenses[i].date + 3, today + 3) == 0) {
            monthTotal += expenses[i].amount;
        }
    }

    printf("\nThis month's spending: Rs %.2f of Rs %.2f budget.\n",
           monthTotal, monthlyBudget);
    if (monthTotal > monthlyBudget) {
        printf("*** WARNING: You have exceeded your monthly budget by Rs %.2f! ***\n",
               monthTotal - monthlyBudget);
    } else if (monthTotal >= 0.8f * monthlyBudget) {
        printf("Note: You have used more than 80%% of your monthly budget.\n");
    }
}

/*
 * deleteBudget: removes the monthly budget (sets it back to 0).
 * With no budget, the warning alerts are switched off.
 */
void deleteBudget(void) {
    printf("\n--- Delete Monthly Budget ---\n");
    if (monthlyBudget <= 0) {
        printf("No budget is set, nothing to delete.\n");
        return;
    }
    printf("Current budget: Rs %.2f\n", monthlyBudget);
    if (confirmAction("Delete the monthly budget? (y/n): ")) {
        monthlyBudget = 0;
        saveData();
        printf("Budget deleted. Budget alerts are now off.\n");
    } else {
        printf("Delete cancelled.\n");
    }
}

/*
 * budgetMenu: small sub-menu for budget settings
 * (set/update, delete, back).
 */
void budgetMenu(void) {
    int choice;

    do {
        printf("\n--- Budget Settings ---\n");
        if (monthlyBudget > 0) {
            printf("Current monthly budget: Rs %.2f\n", monthlyBudget);
        } else {
            printf("No monthly budget is set.\n");
        }
        printf("  1. Set / Update Budget\n");
        printf("  2. Delete Budget\n");
        printf("  3. Back to Main Menu\n");
        choice = readInt("Enter choice: ");

        if (choice == 1) {
            setBudget();
        } else if (choice == 2) {
            deleteBudget();
        } else if (choice != 3) {
            printf("Invalid choice! Please enter 1 to 3.\n");
        }
        if (choice != 3) {
            pauseScreen();
        }
    } while (choice != 3);
}
