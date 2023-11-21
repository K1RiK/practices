// Copyright 2023 Lazarev Kirill

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../practice1/include/stack.h"
#include "../practice1/include/queue.h"
#include "../practice1/include/hash_table.h"
#include "../practice1/include/set.h"


int strinc(char* str, char** list) {  // ������� ��������� ���� �� str � ������ list (in)
    for (int i = 0; i < 3; i++) {
        if (strcmp(str, list[i]) == 0) return 1;
    }
    return 0;
}


int hash(char* str) {  // ���-������� ��� ��������� � �������
    int hash = 0;
    for (int i = 0; str[i] != 0; i++) {
        hash = (hash * 17 + str[i]) % hash_table_lenght;
    }
    return hash;
}


int _main(char* result, char stri[]) {
    // system("chcp 1251");

    // snprintf(result, 4096, "%sstri:%s\n", stri);

    int argc = 1;
    char argv[16][1024] = {""};
    char tmp[64] = "";

    for (int i = 0; stri[i] != '\0'; i++) {
        if (stri[i] == ' ') {
            snprintf(argv[argc], sizeof(argv[argc]), "%s", tmp);
            argc++;
            snprintf(tmp, sizeof(tmp), "");
        } else {
            snprintf(tmp, sizeof(tmp), "%s%c", tmp, stri[i]);
        }
    }
    snprintf(argv[argc], sizeof(argv[argc]), "%s", tmp);
    argc++;

    // snprintf(result, 4096, "%sargc:%d\n", result, argc);


    // for (int i = 0; i < argc; i++) {
    //     snprintf(result, 4096, "%s%d:%s\n", result, i, argv[i]);
    // }

    char* SET_COMMANDS[] = {"SADD", "SREM", "SISMEMBER"};
    char* STACK_COMMANDS[] = {"SPUSH", "SPOP", "SPOP"};
    char* QUEUE_COMMANDS[] = {"QPUSH", "QPOP", "QPOP"};
    char* HASH_TABLE_COMMANDS[] = {"HSET", "HDEL", "HGET"};

    char* file = NULL;
    char* cmd[128];
    char* work = NULL;

    // Stack init
    Stack stack;
    Stack* pstack = &stack;

    // Queue init
    Queue queue;
    Queue* pqueue = &queue;

    // Set init
    Set set;
    set.hash = &hash;
    Set* pset = &set;

    // Hash table init
    Hash_table hash_table;
    hash_table.len = 0;
    hash_table.hash = &hash;
    Hash_table* phash_table = &hash_table;


    if (argc > 2 && strcmp(argv[1], "--file") == 0) {  // ��������� ���� �� ���� --file � ��� ��������
        file = argv[2];
    } else {
        snprintf(result, 4096, "%s������� ���� --file <��� �����>\n", result);
        return 1;
    }

    if (argc > 4 && strcmp(argv[3], "--query") == 0) {  // ��������� ���� �� ���� --query � ��� ��������
        int iswoarg = (strcmp(argv[4], "SPOP") == 0 || strcmp(argv[4], "QPOP") == 0);
        if ((iswoarg && argc == 6) || (!iswoarg && argc == 7) || (strcmp(argv[4], "HSET") == 0 && argc == 8)) {
            for     (int i = 4; i < argc; i++) cmd[i - 4] = argv[i];
            if      (iswoarg && argc == 6)                 cmd[2] = 0;  // ���� ������� �� ������� ��������
            if      (strinc(argv[4], STACK_COMMANDS))      work = "stack";
            else if (strinc(argv[4], QUEUE_COMMANDS))      work = "queue";
            else if (strinc(argv[4], SET_COMMANDS))        work = "set";
            else if (strinc(argv[4], HASH_TABLE_COMMANDS)) work = "hash";
        } else {
            snprintf(result, 4096, "%s������� ���� --query <������>\n", result);
            return 1;
        }
    } else {
        snprintf(result, 4096, "%s������� ���� --query <������>\n", result);
        return 1;
    }

    FILE* fr = fopen(file, "r");
    if (fr == NULL) {
        snprintf(result, 4096, "%s���� �� ������ ��� � ��������� ��� � ���� �������!", result);
        return 1;
    }

    char line[16000];
    char head[64][4096];  // ��������� ���������� ����� �� �������� ������
    char footer[64][4096];  // ��������� ���������� ����� ����� ������
    int find_str = 1, head_line_numbers = 0, footer_line_numbers = 0;

    while (fgets(line, sizeof(line), fr)) {  // ��������� ���� �� �������
        char tmp[4096];
        snprintf(tmp, sizeof(tmp), "%s", line);
        char* arg = strtok(tmp, " ");
        int i = 0, r = 0;
        while (arg && find_str) {
            if (i == 0 && strcmp(arg, work) == 0) r++;  // ���������� ��� �������� � �������� ��������
            else if (i == 1 && strcmp(arg, cmd[1]) == 0) r++;  // ���������� �������� ��������� � ��������
            else if (r == 2) {  // ���� ��� ��������� � �� �������� ��������� � �������
                if (strcmp(work, "stack") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] = 0;  // ������� \n
                        spush(pstack, arg);
                        arg = strtok(NULL, " ");  // ������ ������ �� �������� (slpit)
                    }
                    if      (strcmp(cmd[0], "SPUSH") == 0) spush(pstack, cmd[2]);
                    else if (strcmp(cmd[0], "SPOP") == 0)  snprintf(result, 4096, "%sSPOP:%s\n", result, spop(pstack));
                } else if (strcmp(work, "queue") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] = 0;  // ������� \n
                        qpush(pqueue, arg);
                        arg = strtok(NULL, " ");  // ������ ������ �� �������� (slpit)
                    }
                    if      (strcmp(cmd[0], "QPUSH") == 0) qpush(pqueue, cmd[2]);
                    else if (strcmp(cmd[0], "QPOP") == 0)  snprintf(result, 4096, "%sQPOP:%s\n", result, qpop(pqueue));
                } else if (strcmp(work, "set") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] = 0;  // ������� \n
                        sadd(pset, arg);
                        arg = strtok(NULL, " ");  // ������ ������ �� �������� (slpit)
                    }
                    if      (strcmp(cmd[0], "SADD") == 0)      sadd(pset, cmd[2]);
                    else if (strcmp(cmd[0], "SREM") == 0)      snprintf(result, 4096, "%sSREM:%d\n", result, srem(pset, cmd[2]));
                    else if (strcmp(cmd[0], "SISMEMBER") == 0) snprintf(result, 4096, "%sSISMEMBER:%d\n", result, sismember(pset, cmd[2]));
                } else if (strcmp(work, "hash") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] =  ' ';  // ������� \n
                        char key[4096];
                        char value[4096];
                        char* pkey;
                        char* pvalue;
                        snprintf(key, sizeof(key), "%s", arg);  // �������� �������� arg � key
                        snprintf(value, sizeof(value), "%s", arg);  // �������� �������� arg � value
                        pkey = strtok(key, ":");  // ������ ������ �� : (slpit)
                        pvalue = strtok(value, ":");  // ������ ������ �� : (slpit)
                        pvalue = strtok(NULL, " ");  // ������ ������ �� �������� (slpit)
                        hset(phash_table, pkey, pvalue);
                        arg = strtok(NULL, " ");  // ������ ������ �� �������� (slpit)
                    }
                    if      (strcmp(cmd[0], "HSET") == 0) hset(phash_table, cmd[2], cmd[3]);
                    else if (strcmp(cmd[0], "HDEL") == 0) hdel(phash_table, cmd[2]);
                    else if (strcmp(cmd[0], "HGET") == 0) snprintf(result, 4096, "%sHGET:%s\n", result, hget(phash_table, cmd[2]));
                }
                find_str = 0;  // ������ ������ ������� � ���������
            }
            arg = strtok(NULL, " ");  // ������ ������ �� �������� (slpit)
            i++;
        }
        if (find_str) {
            snprintf(head[head_line_numbers], sizeof(head[head_line_numbers]), "%s", line);
            // snprintf(result, 4096, "%sHEAD[%d]:%s", head_line_numbers, head[head_line_numbers]);
            head_line_numbers++;
        } else {
            snprintf(footer[footer_line_numbers], sizeof(footer[footer_line_numbers]), "%s", line);
            // snprintf(result, 4096, "%sFOOTER[%d]:%s", footer_line_numbers, footer[footer_line_numbers]);
            footer_line_numbers++;
        }
    }
    if (find_str == 1 && cmd[2] != 0) {  // ���� ������ ������ � ��������� ��� � �������� �� ����������
        if      (strcmp(work, "stack") == 0)       spush(pstack, cmd[2]);
        else if (strcmp(work, "queue") == 0)       qpush(pqueue, cmd[2]);
        else if (strcmp(work, "set") == 0 &&
                 strcmp(cmd[0], "SREM") != 0 &&
                 strcmp(cmd[0], "SISMEMBER") != 0) sadd(pset, cmd[2]);
        else if (strcmp(work, "hash") == 0 &&
                 strcmp(cmd[0], "HSET") == 0)      hset(phash_table, cmd[2], cmd[3]);
    }
    fclose(fr);
    FILE* fw = fopen(file, "w");
    if (fw == NULL) {
        snprintf(result, 4096, "%s���� �� ������ ��� � ��������� ��� � ���� �������!", result);
        return 1;
    }
    // �������������� ����
    for     (int i = 0; i < head_line_numbers; i++)                  fputs(head[i], fw);
    if      (strcmp(work, "stack") == 0 && stack.head != 0)          stack_save(cmd[1], pstack, fw);
    else if (strcmp(work, "queue") == 0 && queue.end != queue.start) queue_save(cmd[1], pqueue, fw);
    else if (strcmp(work, "set") == 0 && set.len != 0)               set_save(cmd[1], pset, fw);
    else if (strcmp(work, "hash") == 0 && hash_table.len != 0)       hash_table_save(cmd[1], phash_table, fw);
    for     (int i = 1; i < footer_line_numbers; i++)                fputs(footer[i], fw);
    fclose(fw);

    return 0;
}
