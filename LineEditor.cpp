#include <iostream>
#include <bits/stdc++.h>
using namespace std;

#define MAX_LINES 25

typedef struct Line
{
    char *text;
    Line *next;
    Line *prev;

} Line;

enum OperationType
{
    ADD,
    ADD_W,
    INSERT,
    DELETE,
    UPDATE,
    REPLACE,
    MODIFY
};
struct Operation
{
    OperationType type;
    int lineNumber;
    string oldText;
    string newText;
};
stack<Operation> undoStack;
stack<Operation> redoStack;

Line **buffer[MAX_LINES];
Line *head = NULL;
Line *tail = NULL;
int lines = 0;

void addLine(const char *txt);
void readLines();
void readLineNo(int pos);
void insertLineAt(int pos, const char *txt);
void deleteLine(int pos);
void deleteLast();
void UpdateLine(int pos, const char *txt);
void addWord(int lineNo, int pos, const char *word);
vector<int> searchWord(const char *word);
void updateWord(int lineNo, const char *word, const char *newWord);
void removeLine(int pos);
void replaceWordInLine(int lineNumber, const char *oldWord, const char *newWord);
void undoLastThree();
void redoLastThree();
pair<Line *, int> searchLineWord1(const char *word)
{
    Line *curr = head;
    int line_no = 1;
    int wordLen = strlen(word);

    while (curr != NULL)
    {
        char *sentence = curr->text;
        int sentenceLen = strlen(sentence);

        for (int i = 0; i <= sentenceLen - wordLen; i++)
        {
            // Check if the substring matches the word
            if (strncmp(&sentence[i], word, wordLen) == 0)
            {
                // Ensure it's a whole word by checking boundaries
                bool isWordStart = (i == 0 || sentence[i - 1] == ' ' || sentence[i - 1] == '\t');
                bool isWordEnd = (i + wordLen == sentenceLen || sentence[i + wordLen] == ' ' || sentence[i + wordLen] == '\t' || sentence[i + wordLen] == '\0');

                if (isWordStart && isWordEnd)
                {
                    return {curr, i}; // Return the line and position of the word
                }
            }
        }

        curr = curr->next;
        line_no++;
    }

    return {NULL, -1}; // Return failure if word is not found
}
void ModifyTillInd(int lineNo, const char *word, const char *endword, const char *txt)
{
    pair<Line *, int> p = searchLineWord1(word);
    pair<Line *, int> e = searchLineWord1(endword);
    Line *curr = p.first;
    int start = p.second;
    int end = e.second + strlen(endword); //
    char *sentence = curr->text;
    int slength = strlen(sentence);
    int wlength = strlen(txt);

    if (start < 0 || start > slength || end < start || end > slength)
    {
        cout << "Invalid position" << endl;
        return;
    }

    int newlen = start + wlength + (slength - end) + 1;

    char *newS = (char *)malloc(newlen);

    strncpy(newS, sentence, start);
    newS[start] = '\0';

    strcat(newS, txt);

    strcat(newS, sentence + end);

    Operation op;
    op.type = MODIFY;
    op.lineNumber = lines;
    op.oldText = curr->text;
    free(curr->text);
    curr->text = newS;
    cout << "updated Line: " << curr->text << endl;

    op.newText = curr->text;
    undoStack.push(op);
}
void readFile(FILE *file)
{
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        addLine(line);
    }
}
void writeFile(FILE *file)
{
    Line *curr = head;
    while (curr)
    {
        fprintf(file, "%s", curr->text);
        curr = curr->next;
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
    char word[50], newWord[50], endWord[50];
    int lineNumber, position, start, end;

    while (1)
    {
        printf("Enter a command (add, insert,insertWord, delete, search, replace, modifyTill, print, last, undo, redo, quit): ");
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
            cin >> lineNumber;
            scanf(" %[^\n]", text);

            insertLineAt(lineNumber, text);
        }
        else if (strcmp(command, "insertWord") == 0)
        {
            printf("Enter the line number position , and text: ");
            cin >> lineNumber;
            cin >> position;
            scanf(" %[^\n]", text);

            addWord(lineNumber, position, text);
        }
        else if (strcmp(command, "delete") == 0)
        {
            printf("Enter the line number to delete: ");
            cin >> lineNumber;

            removeLine(lineNumber);
            cout << " Successfully deleted the line . " << endl;
        }
        else if (strcmp(command, "search") == 0)
        {
            printf("Enter the word to search: ");

            scanf(" %[^\n]", word);
            vector<int> li = searchWord(word);
            if (li[0] != -1)
            {
                printf("Found at line %d, position %d\n", li[0], li[1]);
            }
            else
            {
                printf("Word not found.\n");
            }
        }
        else if (strcmp(command, "replace") == 0)
        {
            printf("Enter the line number, old word, and new word: ");
            scanf("%d", lineNumber);
            scanf(" %[^\n]", word);
            scanf(" %[^\n]", newWord);

            replaceWordInLine(lineNumber, word, newWord);
        }

        else if (strcmp(command, "ModifyTill") == 0)
        {
            printf("Enter the line number, starting word, and ending word,new text: ");
            scanf("%d", lineNumber);
            scanf(" %[^\n]", word);
            scanf(" %[^\n]", endWord);

            scanf(" %[^\n]", newWord);

            ModifyTillInd(lineNumber, word, endWord, newWord);
        }
        else if (strcmp(command, "print") == 0)
        {
            readLines();
        }
        else if (strcmp(command, "last") == 0)
        {
            readLineNo(lines);
        }
        else if (strcmp(command, "undo") == 0)
        {
            undoLastThree();
        }
        else if (strcmp(command, "redo") == 0)
        {
            redoLastThree();
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
void addLine(const char *txt)
{
    if (lines >= MAX_LINES)
    {
        cout << "Buffer is full.\n";
        return;
    }
    Line *newLine = (Line *)malloc(sizeof(Line));

    newLine->text = strdup(txt);
    newLine->next = NULL;
    newLine->prev = tail;

    // if there is a line
    if (tail)
    {
        tail->next = newLine;
    }
    else
    {
        // if there is no line , it is the first line
        head = newLine;
    }
    tail = newLine;

    lines++;
    Operation op;
    op.type = ADD;
    op.lineNumber = lines; // New line number
    op.newText = txt;      // Newly added text
    undoStack.push(op);
}
void addWord(int lineNo, int pos, const char *word)
{
    Line *curr = head;
    int line_no = 1;

    while (curr != NULL && line_no < lineNo)
    {

        curr = curr->next;
        line_no++;
    }
    if (curr == NULL)
    {
        cout << "Line does not exist";
        return;
    }

    char *sentence = curr->text;
    int slength = strlen(sentence);
    int wlength = strlen(word);
    if (pos > slength || pos < 0)
    {
        cout << "Invalid position for insertion." << endl;
        return;
    }

    char *newS = (char *)malloc(slength + wlength + 2);

    strncpy(newS, sentence, pos);
    strcpy(newS + pos, word);
    strcpy(newS + pos + wlength, " ");
    strcpy(newS + pos + wlength + 1, sentence + pos);
    free(curr->text);
    curr->text = newS;
    cout << "word inserted successfully   : " << curr->text << endl;
}
void readLineNo(int pos)
{
    Line *curr = head;
    int line_no = 1;

    while (curr != NULL && line_no < pos)
    {

        curr = curr->next;
        line_no++;
    }
    cout << curr->text << endl;
}
void readLines()
{
    int count = 1;
    Line *curr = head;
    while (curr != NULL)
    {
        //[Line No1:]
        cout << "[Line No." << count << ":] " << curr->text << endl;
        curr = curr->next;
        count++;
    }
    free(curr);
    return;
}
vector<int> searchWord(const char *word)
{
    Line *curr = head;
    int line_no = 1;

    while (curr != NULL)
    {
        char *foundPos = strstr(curr->text, word);
        if (foundPos)
        {
            int pos = foundPos - curr->text;
            return {line_no, pos};
        }

        curr = curr->next;
        line_no++;
    }
    return {-1, -1};
}
void updateWord(int lineNo, const char *word, const char *newWord)
{
    vector<int> li = searchWord(word);
    cout << li[1] << endl
         << li[0];

    int pos = li[1];
    if (lineNo == -1 || pos == -1)
    {
        cout << "No such word found for replacement" << endl;
        return;
    }
    else
    {
        Line *curr = head;
        int currentLine = 1;

        // Traverse to the specific line
        while (curr != NULL && currentLine < li[0])
        {
            curr = curr->next;
            currentLine++;
        }
        if (curr == NULL)
        {
            cout << "Invalid line number" << endl;
            return;
        }

        int len = strlen(curr->text) - strlen(word) + strlen(newWord) + 1;

        char *newLine = (char *)malloc(len);

        strncpy(newLine, curr->text, pos);
        newLine[pos] = '\0';
        strcat(newLine, newWord);
        strcat(newLine, curr->text + pos + strlen(word));

        free(curr->text);
        curr->text = newLine;
    }
}

void insertLineAt(int pos, const char *txt)
{
    Line *newline = (Line *)malloc(sizeof(Line));
    newline->text = strdup(txt);
    if (pos > lines + 1)
    {
        cout << "Invalid position " << endl;
    }
    if (pos == 1)
    {
        newline->next = head;
        newline->prev = NULL;
        if (head)
            head->prev = newline;
        else
            tail = newline;

        head = newline;
        lines++;
        return;
    }
    if (pos == lines + 1)
    {
        newline->next = NULL;
        newline->prev = tail;
        if (tail)
            tail->next = newline;
        else
        {
            head = newline;
        }
        tail = newline;
        lines++;
        return;
    }

    Line *curr = head;
    Line *previous = head;
    int line_no = 1;

    while (curr != NULL && line_no < pos)
    {
        previous = curr;
        curr = curr->next;
        line_no++;
    }
    previous->next = newline;
    newline->prev = previous;
    newline->next = curr;
    if (curr)
    {
        curr->prev = newline;
    }
    lines++;
    Operation op;
    op.type = ADD;
    op.lineNumber = lines; // New line number
    op.newText = txt;      // Newly added text
    undoStack.push(op);
}

void deleteLast()
{
    if (head == tail)
    {
        head = NULL;
        tail = NULL;
        return;
    }
    if (head == NULL)
    {
        cout << " No lines to delete";
        return;
    }
    Line *curr = tail;
    tail = curr->prev;
    tail->next = NULL;
    lines--;
    free(curr);

    return;
}
void removeLine(int pos)
{
    if (head == NULL || pos > lines || pos < 1)
    {
        cout << "No lines to remove Or Invalid Position of line" << endl;
        return;
    }
    else if (pos == lines)
    {
        deleteLast();
    }
    else if (pos == 1)
    {
        Line *rem = head;
        head = head->next;
        head->prev = NULL;
        free(rem);
        lines--;
        return;
    }

    else
    {
        Line *curr = head;
        Line *previous = head;
        int line_no = 1;

        while (curr != NULL && line_no < pos)
        {
            previous = curr;
            curr = curr->next;
            line_no++;
        }
        char *oldText = curr->text;
        previous->next = curr->next;
        curr->next->prev = previous;
        lines--;

        free(curr);
        Operation op;
        op.type = DELETE;
        op.lineNumber = pos;
        op.oldText = oldText; // Deleted text
        undoStack.push(op);
    }
    return;
}
void removeWord(int ln, const char *word)
{
    Operation op;
    op.oldText = strdup(word);

    vector<int> li = searchWord(word);
    int lineNo = li[0];
    int pos = li[1];

    if (lineNo == -1 || pos == -1)
    {
        cout << "No such word found for removal" << endl;
        return;
    }

    Line *curr = head;
    int currentLine = 1;

    while (curr != NULL && currentLine < lineNo)
    {
        curr = curr->next;
        currentLine++;
    }

    if (curr == NULL || curr->text == NULL)
    {
        cout << "Invalid line or empty text." << endl;
        return;
    }

    int wordLen = strlen(word);
    int lineLen = strlen(curr->text);

    char *newLine = (char *)malloc(lineLen - wordLen + 1);

    if (newLine == NULL)
    {
        cout << "Memory allocation failed." << endl;
        return;
    }

    strncpy(newLine, curr->text, pos);

    strcpy(newLine + pos, curr->text + pos + wordLen);

    free(curr->text);

    curr->text = newLine;

    cout << "Word removed successfully." << endl;

    op.newText = strdup(curr->text);
}

pair<Line *, int> searchLineWord(const char *word)
{
    Line *curr = head;
    int line_no = 1;

    while (curr != NULL)
    {
        char *foundPos = strstr(curr->text, word);
        if (foundPos)
        {
            int pos = foundPos - curr->text;
            return {curr, pos};
        }

        curr = curr->next;
        line_no++;
    }
    return {NULL, -1};
}
void replaceWordInLine(int lineNumber, const char *oldWord, const char *newWord)
{
    pair<Line *, int> p = searchLineWord1(oldWord);

    Line *curr = p.first;

    if (curr == NULL || p.second == -1)
    {
        cout << "Word not found in the line." << endl;
        return;
    }

    int pos = p.second;

    int oldWordLen = strlen(oldWord);
    int newWordLen = strlen(newWord);
    int originalLineLen = strlen(curr->text);
    int newLength = originalLineLen - oldWordLen + newWordLen + 1;

    char *newLine = (char *)malloc(newLength);

    strncpy(newLine, curr->text, pos);
    newLine[pos] = '\0';

    strcat(newLine, newWord);

    strcat(newLine, curr->text + pos + oldWordLen);

    Operation op;
    op.oldText = curr->text;

    free(curr->text);
    curr->text = newLine;

    op.type = REPLACE;
    op.lineNumber = lineNumber;
    op.newText = curr->text;
    undoStack.push(op);

    cout << "Replacement successful. Updated line: " << curr->text << endl;
}
void undoLastThree()
{

    int count = 0;
    while (!undoStack.empty() && count < 3)
    {

        Operation lastOp = undoStack.top();
        undoStack.pop();

        switch (lastOp.type)
        {
        case ADD:

            removeLine(lastOp.lineNumber);
            break;
        case ADD_W:

            removeWord(lastOp.lineNumber, lastOp.newText.c_str());
            break;

        case DELETE:

            insertLineAt(lastOp.lineNumber, lastOp.oldText.c_str());
            break;

        case UPDATE:

            break;

        case REPLACE:

            break;

        case MODIFY:

            break;

        default:
            cout << "Unknown operation type." << endl;
            return;
        }

        redoStack.push(lastOp);
        count++;
    }

    if (count == 0)
    {
        cout << "No actions to undo." << endl;
    }
}
void redoLastThree()
{

    int count = 0;
    while (!redoStack.empty() && count < 3)
    {

        Operation lastOp = redoStack.top();
        redoStack.pop();

        switch (lastOp.type)
        {
        case ADD:

            insertLineAt(lastOp.lineNumber, lastOp.newText.c_str());
            break;

        case DELETE:

            removeLine(lastOp.lineNumber);
            break;

        case UPDATE:

            break;

        case REPLACE:

            break;

        case MODIFY:

            break;

        default:
            cout << "Unknown operation type." << endl;
            return;
        }

        undoStack.push(lastOp);
        count++;
    }

    if (count == 0)
    {
        cout << "No actions to redo." << endl;
    }
}