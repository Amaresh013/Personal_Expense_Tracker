/*
 * expense.h  -  Shared header file for the Personal Expense Tracker
 * Every .c file includes this file so they all know about the same
 * struct, constants, global variables and functions.
 * "#ifndef ... #endif" (include guard) stops it being included twice.
 */
#ifndef EXPENSE_H
#define EXPENSE_H

#include <stdio.h>    /* printf, fgets, fopen, fread, fwrite, fclose */
#include <stdlib.h>   /* strtol, strtod, exit                        */
#include <string.h>   /* strlen, strcmp, strcpy                      */
#include <time.h>     /* time, localtime (today's date)              */
#include <ctype.h>    /* tolower, isalnum (text checking)            */

/* ---------------- Constants ---------------- */
#define MAX_EXPENSES    500         /* maximum expense records               */
#define DATE_LEN        11          /* "DD-MM-YYYY" = 10 chars + '\0'        */
#define CAT_LEN         20          /* max length of a category name         */
#define DESC_LEN        50          /* max length of description             */
#define MAX_CATEGORIES  15          /* maximum number of categories          */
#define MAX_USERS       10          /* maximum number of admins              */
#define USER_LEN        20          /* max length of username                */
#define PASS_LEN        20          /* max length of password                */
#define DATA_FILE       "expenses.dat"   /* expense records + budget         */
#define USER_FILE       "users.dat"      /* admin accounts                   */
#define CAT_FILE        "categories.dat" /* category list                    */

/* ---------------- Data Structures ---------------- */
/* One expense record. */
struct Expense {
    int   id;                       /* unique number, never repeats          */
    char  date[DATE_LEN];           /* date in DD-MM-YYYY format             */
    char  category[CAT_LEN];        /* Food, Education, ...                  */
    float amount;                   /* money spent (always > 0)              */
    char  description[DESC_LEN];    /* short note about the expense          */
};

/* One admin account. Password and answer are stored as hash numbers,
 * never as plain text. */
struct User {
    char          username[USER_LEN];
    unsigned long passHash;         /* hash of the password                  */
    int           questionNo;       /* which security question was chosen    */
    unsigned long answerHash;       /* hash of the security answer           */
};

/* ---------------- Global Variables ---------------- */
/* "extern" means: these are created in main.c, other files just use them. */
extern struct Expense expenses[MAX_EXPENSES];
extern int   expenseCount;
extern int   nextId;
extern float monthlyBudget;                     /* 0 = budget not set        */
extern struct User users[MAX_USERS];
extern int   userCount;
extern char  currentUser[USER_LEN];             /* who is logged in now      */
extern char  categories[MAX_CATEGORIES][CAT_LEN];
extern int   categoryCount;

/* ---------------- Function Prototypes ---------------- */
/* input.c - safe keyboard input */
int   readLine(char *buffer, int size);
void  readText(const char *prompt, char *buffer, int size);
int   readInt(const char *prompt);
float readAmount(const char *prompt);
void  readDate(char *date);
int   isValidDate(const char *date);
int   isBlank(const char *text);
int   confirmAction(const char *prompt);
void  chooseCategory(char *category);
void  getTodayDate(char *date);
void  pauseScreen(void);

/* storage.c - file handling */
void  loadData(void);
void  saveData(void);
void  loadUsers(void);
void  saveUsers(void);
void  loadCategories(void);
void  saveCategories(void);

/* expense.c - add / view / search / update / delete */
int   findIndexById(int id);
void  printHeader(void);
void  printRow(const struct Expense *e);
void  addExpense(void);
void  viewExpenses(void);
void  searchExpense(void);
void  updateExpense(void);
void  deleteExpense(void);

/* report.c - summary and budget */
void  calculateSummary(void);
void  budgetMenu(void);
void  setBudget(void);
void  deleteBudget(void);
void  checkBudget(void);

/* category.c - manage categories */
void  initDefaultCategories(void);
void  viewCategories(void);
void  addCategory(void);
void  deleteCategory(void);
void  categoryMenu(void);

/* auth.c - login, logout support, register, recovery */
int   loginMenu(void);
void  addNewAdmin(void);

#endif
