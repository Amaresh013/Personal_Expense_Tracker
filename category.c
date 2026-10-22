/* category.c - add, view and delete expense categories.
 * The category list is saved in categories.dat. */
#include "expense.h"

/* equalIgnoreCase: compares two texts ignoring capital/small letters.
 * Returns 1 if they are the same. Used to stop duplicate categories. */
static int equalIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == *b;                           /* both must end together    */
}

/* initDefaultCategories: sets the 5 starting categories. */
void initDefaultCategories(void) {
    const char *defaults[] = {"Food", "Education", "Transportation",
                              "Shopping", "Bills"};
    int i;
    categoryCount = 5;
    for (i = 0; i < categoryCount; i++) {
        strcpy(categories[i], defaults[i]);
    }
}

/* viewCategories: prints the numbered category list. */
void viewCategories(void) {
    int i;
    printf("\n--- Categories (%d of %d) ---\n", categoryCount, MAX_CATEGORIES);
    for (i = 0; i < categoryCount; i++) {
        printf("  %d. %s\n", i + 1, categories[i]);
    }
}

/* addCategory: adds a new category name to the list. */
void addCategory(void) {
    char name[64];                             /* big buffer to catch long names */
    int i;

    printf("\n--- Add New Category ---\n");
    if (categoryCount >= MAX_CATEGORIES) {
        printf("Category list is full (maximum %d).\n", MAX_CATEGORIES);
        return;
    }
    readText("Enter new category name: ", name, sizeof(name));

    if (isBlank(name)) {
        printf("Category name cannot be empty.\n");
        return;
    }
    if (strlen(name) >= CAT_LEN) {
        printf("Name too long. Maximum %d characters.\n", CAT_LEN - 1);
        return;
    }
    for (i = 0; i < categoryCount; i++) {      /* check for duplicate       */
        if (equalIgnoreCase(categories[i], name)) {
            printf("Category '%s' already exists.\n", categories[i]);
            return;
        }
    }
    strcpy(categories[categoryCount], name);   /* safe: length checked above */
    categoryCount++;
    saveCategories();
    printf("Category '%s' added successfully.\n", name);
}

/*
 * deleteCategory: removes a category.
 * A category that is used by any expense cannot be deleted, so old
 * records never point to a missing category.
 */
void deleteCategory(void) {
    int choice, i, used = 0;

    viewCategories();
    printf("\n--- Delete Category ---\n");
    if (categoryCount <= 1) {
        printf("At least one category must remain.\n");
        return;
    }
    choice = readInt("Enter category number to delete (0 to cancel): ");
    if (choice == 0) {
        printf("Delete cancelled.\n");
        return;
    }
    if (choice < 1 || choice > categoryCount) {
        printf("Invalid category number.\n");
        return;
    }
    choice--;                                  /* array index starts at 0   */

    for (i = 0; i < expenseCount; i++) {       /* count records using it    */
        if (strcmp(expenses[i].category, categories[choice]) == 0) {
            used++;
        }
    }
    if (used > 0) {
        printf("Cannot delete '%s': %d expense record(s) use it.\n",
               categories[choice], used);
        printf("Update or delete those expenses first.\n");
        return;
    }
    if (confirmAction("Are you sure you want to delete this category? (y/n): ")) {
        printf("Category '%s' deleted.\n", categories[choice]);
        for (i = choice; i < categoryCount - 1; i++) {
            strcpy(categories[i], categories[i + 1]);   /* shift left       */
        }
        categoryCount--;
        saveCategories();
    } else {
        printf("Delete cancelled.\n");
    }
}

/* categoryMenu: sub-menu for managing categories. */
void categoryMenu(void) {
    int choice;

    do {
        printf("\n--- Manage Categories ---\n");
        printf("  1. View Categories\n");
        printf("  2. Add New Category\n");
        printf("  3. Delete Category\n");
        printf("  4. Back to Main Menu\n");
        choice = readInt("Enter choice: ");

        if (choice == 1)      viewCategories();
        else if (choice == 2) addCategory();
        else if (choice == 3) deleteCategory();
        else if (choice != 4) printf("Invalid choice! Please enter 1 to 4.\n");

        if (choice != 4) {
            pauseScreen();
        }
    } while (choice != 4);
}
