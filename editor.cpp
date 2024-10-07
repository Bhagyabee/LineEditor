#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#define MAX_LINES 25

typedef struct Line
{
    char *text;
} Line;

typedef struct Cursor
{
    int line;
    int position;
} Cursor;

Line buffer[MAX_LINES];
int lineCount = 0;

// Add a new line to the buffer
void addLine(char *text)
{
    if (lineCount >= MAX_LINES)
    {
        printf("Buffer is full.\n");
        return;
    }
    buffer[lineCount].text = strdup(text);
    lineCount++;
}

// Read a file into the buffer
void readFile(FILE *file)
{
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        addLine(line);
    }
}

// Write the buffer contents to a file
void writeFile(FILE *file)
{
    for (int i = 0; i < lineCount; i++)
    {
        fprintf(file, "%s", buffer[i].text);
    }
}

// Search for a word and return the line and position
Cursor searchWord(const char *word)
{
    Cursor cursor = {-1, -1};
    for (int i = 0; i < lineCount; i++)
    {
        char *pos = strstr(buffer[i].text, word);
        if (pos != NULL)
        {
            cursor.line = i;
            cursor.position = pos - buffer[i].text;
            return cursor;
        }
    }
    return cursor; // Word not found
}

// Insert a line at a specific position
void insertLine(int position, char *newText)
{
    if (lineCount >= MAX_LINES || position < 0 || position > lineCount)
    {
        printf("Invalid position or buffer is full.\n");
        return;
    }
    for (int i = lineCount; i > position; i--)
    {
        buffer[i] = buffer[i - 1];
    }
    buffer[position].text = strdup(newText);
    lineCount++;
}

// Insert a word in a line at a specific position
void insertWord(int lineNo, const char *word, int pos)
{
    if (lineNo < 0 || lineNo >= lineCount)
    {
        printf("Invalid line number.\n");
        return;
    }
    char *line = buffer[lineNo].text;
    char newLine[512];
    snprintf(newLine, pos + 1, "%s", line); // Copy text before the position
    strcat(newLine, word);                  // Insert word
    strcat(newLine, line + pos);            // Add the rest of the original line
    free(buffer[lineNo].text);
    buffer[lineNo].text = strdup(newLine);
}

// Replace a word in a specific line
void replaceWord(int lineNo, const char *oldWord, const char *newWord)
{
    if (lineNo < 0 || lineNo >= lineCount)
    {
        printf("Invalid line number.\n");
        return;
    }
    char *line = buffer[lineNo].text;
    char *pos = strstr(line, oldWord);
    if (pos != NULL)
    {
        char newLine[512];
        strncpy(newLine, line, pos - line);
        newLine[pos - line] = '\0';
        strcat(newLine, newWord);
        strcat(newLine, pos + strlen(oldWord));
        free(buffer[lineNo].text);
        buffer[lineNo].text = strdup(newLine);
    }
    else
    {
        printf("Word not found.\n");
    }
}

// Delete a line from the buffer
void deleteLine(int lineNo)
{
    if (lineNo < 0 || lineNo >= lineCount)
    {
        printf("Invalid line number.\n");
        return;
    }
    free(buffer[lineNo].text);
    for (int i = lineNo; i < lineCount - 1; i++)
    {
        buffer[i] = buffer[i + 1];
    }
    lineCount--;
}

// Print the buffer
void printBuffer()
{
    for (int i = 0; i < lineCount; i++)
    {
        printf("%d: %s", i + 1, buffer[i].text);
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    FILE *file;
    char filePath[100];

    if (argc == 1)
    {
        strcpy(filePath, "file.txt");
        file = fopen(filePath, "w+");
    }
    else if (argc == 2)
    {
        file = fopen(argv[1], "r+");
        if (file == NULL)
        {
            file = fopen(argv[1], "w+");
        }
    }
    else if (argc == 3)
    {
        snprintf(filePath, sizeof(filePath), "%s/%s", argv[2], argv[1]);
        file = fopen(filePath, "r+");
        if (file == NULL)
        {
            file = fopen(filePath, "w+");
        }
    }
    else
    {
        printf("Error: Too many arguments.\n");
        return -1;
    }

    if (!file)
    {
        printf("Error opening file.\n");
        return -1;
    }

    // Read file and store in buffer
    readFile(file);
    fclose(file);

    // Command loop
    char command[20];
    char text[100];
    char word[50], newWord[50];
    int lineNumber, position;

    while (1)
    {
        printf("Enter a command (add, insert, delete, search, replace, print, undo, quit): ");
        scanf("%s", command);

        if (strcmp(command, "add") == 0)
        {
            printf("Enter the text: ");
            scanf(" %[^\n]", text);

            addLine(text);
        }
        else if (strcmp(command, "insert") == 0)
        {
            printf("Enter the line number and text: ");
            scanf("%d %[^\n]", &lineNumber, text);

            insertLine(lineNumber - 1, text);
        }
        else if (strcmp(command, "delete") == 0)
        {
            printf("Enter the line number to delete: ");
            scanf("%d", &lineNumber);

            deleteLine(lineNumber - 1);
        }
        else if (strcmp(command, "search") == 0)
        {
            printf("Enter the word to search: ");
            scanf("%s", word);
            Cursor cursor = searchWord(word);
            if (cursor.line != -1)
            {
                printf("Found at line %d, position %d\n", cursor.line + 1, cursor.position);
            }
            else
            {
                printf("Word not found.\n");
            }
        }
        else if (strcmp(command, "replace") == 0)
        {
            printf("Enter the line number, old word, and new word: ");
            scanf("%d %s %s", &lineNumber, word, newWord);

            replaceWord(lineNumber - 1, word, newWord);
        }
        else if (strcmp(command, "print") == 0)
        {
            printBuffer();
        }
        else if (strcmp(command, "quit") == 0)
        {
            // Save the buffer back to file
            file = fopen(filePath, "w");
            writeFile(file);
            fclose(file);
            break;
        }
        else
        {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
