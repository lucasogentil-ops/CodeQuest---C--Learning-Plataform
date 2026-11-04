#include <stdio.h>
#include <stdbool.h>

// This Lab is preparation for a coding exam on Friday 2nd October @ 3pm in D422 and D417.


// Q1: Check if an inputted year is a Leap Year.
// A leap year is divisible by 4, but not by 100 unless also divisible by 400.
bool isLeapYear(int year) {
    return true;
}

// Q2 (helper): Reverse the digits of a number and return the reversed integer.
// E.g., 123 -> 321.
int Reversed(int testNumber) {
    return 0;
}

// Q2: Check Whether a Number is Palindrome or Not.
// A palindrome reads the same forwards and backwards (e.g., 121).
bool isAPalindrome(int testNumber) {
    return false;
}

// Q3: Check Whether a Number is Prime or Not.
// A prime number is only divisible by itself and 1.
bool isAPrimeNumber(int numbertoTest) {
    return false;
}

// Q4: Convert Binary Number to Decimal without using the shift operator.
// E.g., 110 -> 6.
int convertBinarytoDecimal(int binaryNumber) {
    return 0;
}

// Q6: Output a right-angled triangle of 'A's using 2 nested for loops:
// A
// AA
// AAA
// AAAA
void drawRightAngledTriangle() {
}

// Q7: Output an isosceles (diamond-like) triangle of 'A's using 2 for loops
// and at most 1 if/else:
// A
// AA
// AAA
// AAAA
// AAA
// AA
// A
void drawIsocelesTriangle() {
}

// Q19 (EXTRA): Output the same diamond pattern as Q7 using only 2 for loops
// and no conditional statements.
void drawIsocelesTriangle2() {
}

// Q8: Find an element in an array and print the index where it was found,
// or -1 if not found.
int find(int size, int arr[], int toFind) {
    return -1;
}

// Q9: Find and return the second largest element in an array of positive
// integers; returns -1 if it can't find one.
int find2ndLargest(int size, int arr[]) {
    return -1;
}

// Q10: Copy all elements from arr1 to arr2. Both arrays are the same size.
void copyArraytoArray(int size, int arr1[], int arr2[]) {
    return;
}

// Q11: Insert an element at a specified index in the array, shifting elements
// to the right. 'count' tracks the number of elements and cannot exceed 'size'.
// Returns true if an element was inserted, otherwise false.
bool insertElement(int& size, int& count, int arr[], int elementToInsert, int insertIndex) {
    return false;
}

// Q12: Delete an element at a specified position, shifting higher elements
// to the left by one. 'count' tracks the number of elements and cannot exceed
// 'size'. Returns true if an element was deleted, otherwise false.
bool deleteElement(int& size, int& count, int arr[], int deleteIndex) {
    return true;
}

// Q13: Count the frequency of a given element in an array.
int frequencyCount(int size, int arr[], int value) {
    return 0;
}

// Q14: Count the total number of duplicate elements in an array.
int countDuplicates(int size, int arr[]) {
    return 0;
}

// Q15: Reverse an array in place using only 1 array.
void reverse(int size, int arr[]) {
    return;
}

// Q16: Left rotate an array by one position, moving the first element to the end.
int rotateLeft(int size, int arr[]) {
    return -1;
}

// Q17: Given a flight length and an array of movie lengths, return true if
// there are exactly two movies whose runtimes sum to the flight length.
// Users watch exactly two movies and cannot watch the same movie twice.
bool twoMovies(int flightLength, int movieLengths[], int size) {
    return false;
}
// Q18: Count the number of separate words in an array of chars, where a word
// is one or more characters in ranges A..Z or a..z separated by spaces.
int wordCounter(int size, char characters[]) {
    return 0;
}
// ----------------------------------------------------------------------------
// Q19: PrintArray
// Prints the 4x6 array in rows and columns.
// This is the ONLY function allowed to use printf.
// ----------------------------------------------------------------------------
void PrintArray(int array[4][6])
{
}

// ----------------------------------------------------------------------------
// Q20: ReplaceWithMinusOne
// Scans the array from position (0,0), moving left-to-right across each row.
// At each position, checks for a HORIZONTAL line of 3+ identical items first,
// and if none found, checks for a VERTICAL line of 3+ identical items.
// The first cluster found is scored by summing its values (1-4), then the
// cluster is replaced with -1's. Returns the score, or 0 if no cluster found.
// 0's and -2's are never part of a cluster.
// ----------------------------------------------------------------------------
int ReplaceWithMinusOne(int array[4][6])
{
    return 0;
}

// ----------------------------------------------------------------------------
// Q21:FallDownAndReplace
// Removes the -1's by making all items above them "fall down" into the empty
// space. New items added at the top of the array are given the value -2, and
// these will not be part of the next cluster.
// Works column by column.
// ----------------------------------------------------------------------------
void FallDownAndReplace(int array[4][6])
{
}


// Q21:----------------------------------------------------------------------------
// FindAllVariables
// Scans the C code string and fills 'variables' with a comma-separated list
// of every variable name declared as type "int" or "char".
// Only one variable is declared per line, and strings are 0-terminated.
// E.g., "int var1=10;" adds "var1" to the list.
// ----------------------------------------------------------------------------
void FindAllVariables(char variables[], char code[])
{
}

// Q22----------------------------------------------------------------------------
// FindAllVariablesInScope
// Fills 'variables' with a comma-separated list of variables that are in
// scope AFTER the given lineNumber. The code has no "for" loops, and the
// only function declaration is "int main()". Only one variable is declared
// per line, and only "int" / "char" variables are considered.
//
// Scope is tracked with a brace depth counter. A variable declared at depth
// D is in scope after its declaration line as long as the current depth is
// >= D. A declaration at the same depth as the target line counts; a
// declaration at a deeper depth (inside a block that closes before/at the
// target line) does not.
// ----------------------------------------------------------------------------
void FindAllVariablesInScope(char variables[], char code[], int lineNumber)
{
}



// Very simple tests for Questions 1 to 8..and 19 to 22 . Add Test for Questions 9 to 18 
int main() {
    // example of printf()
    // example printf("Leapyear\n");
    printf("Leapyear\n");
    printf("Leapyear\n");
    if (!isLeapYear(1700))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (isLeapYear(1600))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (isLeapYear(2016))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    printf("NumberReversed\n");
    if (Reversed(123) == 321)
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (Reversed(1) == 1)
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (Reversed(1234) != 321)
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    printf("Palindrome\n");
    if (isAPalindrome(1))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (isAPalindrome(121))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (!isAPalindrome(1213))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    printf("Prime Number\n");
    if (isAPrimeNumber(3))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (isAPrimeNumber(2147483647))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (!isAPrimeNumber(4))
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    printf("Binary to Decimal\n");
    if (convertBinarytoDecimal(110) == 6)
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    if (convertBinarytoDecimal(111) == 7)
    {
        printf("yes\n");
    }
    else {
        printf("no\n");
    }
    printf("Triangles\n");
    drawRightAngledTriangle();
    drawIsocelesTriangle();
    //example of calling the functions

    int array[4][6] = { {0,0,3,1,3,4},
                       {0,0,2,3,4,3},
                       {0,0,1,3,3,2},
                       {0,0,1,1,1,1} };
    PrintArray(array);
    int score = ReplaceWithMinusOne(array);

    FallDownAndReplace(array);
    char code[1000] = "";

    char variables[1000] = "";

    FindAllVariables(variables, code);
    FindAllVariablesInScope(variables, code, 12);

    return 0;
}