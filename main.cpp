#include <stdio.h>
#include <assert.h>
#include <cerrno>

#include "stack.h"

int getFileSize(const char *inPath);

int readFile(const char inPath[], char *text, size_t textSize);

int stkPop(Stack_t *stk);
;
int main() {
    char binFilePath[FILENAME_MAX] = "../../asmCompiler/program-v1.bin";

    size_t binSize = getFileSize(binFilePath);
    char *buffer = (char *) calloc(binSize, sizeof(char) + 1);
    char *cur = buffer;

    if (readFile(binFilePath, buffer, binSize)) perror("Error:");

    Stack_t cpuStk = {};
    Stack_t callStk = {};
    int registers[4] = {};

    stackConstruct(&cpuStk);
    stackConstruct(&callStk);

    while (cur < buffer + binSize) {
#define DEF_CMD(name, num, argType, code, asmCode) \
        case num: code;

#define DEF_CJ(func) \
        int b = stkPop(&cpuStk); \
        int a = stkPop(&cpuStk); \
        \
        if (a func b) { \
            cur = buffer + *((int *)(++cur)); \
        } else { \
            cur += sizeof(char) + sizeof(int); \
        } \
        \
        stackPush(&cpuStk, a); \
        stackPush(&cpuStk, b);

        bool endChecker = false;
        switch (*cur) {

#include"../asmCompiler/commands.h"
            default: printf("Syntax error\n");
        }
        if (endChecker) break;
//        stackDump(&cpuStk);
    }
#undef DEF_CJ
#undef DEF_CMD

    stackDump(&cpuStk);
    free(buffer);
    return 0;
}

int getFileSize(const char *inPath) {
    assert(inPath != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fseek(myFile, 0, SEEK_END);
    size_t textSize = ftell(myFile);
    fclose(myFile);
    return textSize;
}

int readFile(const char inPath[], char *text, size_t textSize) {
    assert(inPath != "");
    assert(text != nullptr);

    FILE *myFile = fopen(inPath, "r");
    if (!myFile) {
        perror("File opening failed");
        return errno;
    }
    fread(text, 1, textSize, myFile);
    fclose(myFile);
    return 0;
}

int stkPop(Stack_t *stk) {
    int val = 0;
    stackPop(stk, &val);

    return val;
}

