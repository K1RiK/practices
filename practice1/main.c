// Copyright 2023 Lazarev Kirill

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/stack.h"
#include "include/queue.h"


int strinc(char* str, char** list) {  // Функция проверяет есть ли str в списке list (in)
    for (int i = 0; i < 3; i++) {
        if (strcmp(str, list[i]) == 0) return 1;
    }
    return 0;
}


int hash(char* str) {  // Хэш-функция для множества и таблицы
    int hash = 0;
    for (int i = 0; str[i] != 0; i++) {
        hash = (hash * 17 + str[i]) % 256;
    }
    return hash;
}

#include "include/set.h"
#include "include/hash_table.h"

int main(int argc, char* argv[]) {
    // system("chcp 1251");

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


    if (argc > 2 && strcmp(argv[1], "--file") == 0) {  // Проверяем есть ли флаг --file и его значение
        file = argv[2];
    } else {
        printf("Укажите флаг --file <имя файла>\n");
        return 1;
    }

    if (argc > 4 && strcmp(argv[3], "--query") == 0) {  // Проверяем есть ли флаг --query и его значение
        int iswoarg = (strcmp(argv[4], "SPOP") == 0 || strcmp(argv[4], "QPOP") == 0);
        if ((iswoarg && argc == 6) || (!iswoarg && argc == 7) || (strcmp(argv[4], "HSET") == 0 && argc == 8)) {
            for     (int i = 4; i < argc; i++) cmd[i - 4] = argv[i];
            if      (iswoarg && argc == 6)                 cmd[2] = 0;  // Если команда не требует значения
            if      (strinc(argv[4], STACK_COMMANDS))      work = "stack";
            else if (strinc(argv[4], QUEUE_COMMANDS))      work = "queue";
            else if (strinc(argv[4], SET_COMMANDS))        work = "set";
            else if (strinc(argv[4], HASH_TABLE_COMMANDS)) work = "hash";
        } else {
            printf("Укажите флаг --query <запрос>\n");
            return 1;
        }
    } else {
        printf("Укажите флаг --query <запрос>\n");
        return 1;
    }

    FILE* fr = fopen(file, "r");
    if (fr == NULL) {
        printf("Файл не найден или у программы нет к нему доступа!");
        return 1;
    }

    char line[1024];
    char head[64][1024];  // Сохраняем содержимое файла до найденой строки
    char footer[64][1024];  // Сохраняем содержимое файла после строки
    int find_str = 1, head_line_numbers = 0, footer_line_numbers = 0;

    while (fgets(line, sizeof(line), fr)) {  // Считываем файл по строкам
        char tmp[1024];
        snprintf(tmp, sizeof(tmp), "%s", line);
        char* arg = strtok(tmp, " ");
        int i = 0, r = 0;

        while (arg && find_str) {
            if (i == 0 && strcmp(arg, work) == 0) r++;  // Сравниваем тип струтуры с введеной командой
            else if (i == 1 && strcmp(arg, cmd[1]) == 0) r++;  // Сравниваем название структуры с введеным
            else if (r == 2) {  // Если тип структуры и ее название совпадают с нужными
                if (strcmp(work, "stack") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] = 0;  // Удаляем \n
                        spush(pstack, arg);
                        arg = strtok(NULL, " ");  // Парсим строку по пробелам (slpit)
                    }
                    if      (strcmp(cmd[0], "SPUSH") == 0) spush(pstack, cmd[2]);
                    else if (strcmp(cmd[0], "SPOP") == 0)  printf("SPOP:%s\n", spop(pstack));
                } else if (strcmp(work, "queue") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] = 0;  // Удаляем \n
                        qpush(pqueue, arg);
                        arg = strtok(NULL, " ");  // Парсим строку по пробелам (slpit)
                    }
                    if      (strcmp(cmd[0], "QPUSH") == 0) qpush(pqueue, cmd[2]);
                    else if (strcmp(cmd[0], "QPOP") == 0)  printf("QPOP:%s\n", qpop(pqueue));
                } else if (strcmp(work, "set") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] = 0;  // Удаляем \n
                        sadd(pset, arg);
                        arg = strtok(NULL, " ");  // Парсим строку по пробелам (slpit)
                    }
                    if      (strcmp(cmd[0], "SADD") == 0)      sadd(pset, cmd[2]);
                    else if (strcmp(cmd[0], "SREM") == 0)      printf("SREM:%d\n", srem(pset, cmd[2]));
                    else if (strcmp(cmd[0], "SISMEMBER") == 0) printf("SISMEMBER:%d\n", sismember(pset, cmd[2]));
                } else if (strcmp(work, "hash") == 0) {
                    while (arg) {
                        arg[strcspn(arg, "\n")] =  ' ';  // Удаляем \n
                        char key[128];
                        char value[128];
                        char* pkey;
                        char* pvalue;
                        snprintf(key, sizeof(key), "%s", arg);  // Копируем значение arg в key
                        snprintf(value, sizeof(value), "%s", arg);  // Копируем значение arg в value
                        pkey = strtok(key, ":");  // Парсим строку по : (slpit)
                        pvalue = strtok(value, ":");  // Парсим строку по : (slpit)
                        pvalue = strtok(NULL, " ");  // Парсим строку по пробелам (slpit)
                        hset(phash_table, pkey, pvalue);
                        arg = strtok(NULL, " ");  // Парсим строку по пробелам (slpit)
                    }
                    if      (strcmp(cmd[0], "HSET") == 0) hset(phash_table, cmd[2], cmd[3]);
                    else if (strcmp(cmd[0], "HDEL") == 0) hdel(phash_table, cmd[2]);
                    else if (strcmp(cmd[0], "HGET") == 0) printf("HGET:%s\n", hget(phash_table, cmd[2]));
                }
                find_str = 0;  // Нужная строка найдена в документе
            }
            arg = strtok(NULL, " ");  // Парсим строку по пробелам (slpit)
            i++;
        }

        if (find_str) {
            snprintf(head[head_line_numbers], sizeof(head[head_line_numbers]), "%s", line);
            // printf("HEAD[%d]:%s", head_line_numbers, head[head_line_numbers]);
            head_line_numbers++;
        } else {
            snprintf(footer[footer_line_numbers], sizeof(footer[footer_line_numbers]), "%s", line);
            // printf("FOOTER[%d]:%s", footer_line_numbers, footer[footer_line_numbers]);
            footer_line_numbers++;
        }
    }

    if (find_str == 1 && cmd[2] != 0) {  // Если нужной строки в документе нет и операция на добавление
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
        printf("Файл не найден или у программы нет к нему доступа!");
        return 1;
    }
    // Перезаписываем файл
    for     (int i = 0; i < head_line_numbers; i++)                  fputs(head[i], fw);
    if      (strcmp(work, "stack") == 0 && stack.head != 0)          stack_save(cmd[1], pstack, fw);
    else if (strcmp(work, "queue") == 0 && queue.end != queue.start) queue_save(cmd[1], pqueue, fw);
    else if (strcmp(work, "set") == 0 && set.len != 0)               set_save(cmd[1], pset, fw);
    else if (strcmp(work, "hash") == 0 && hash_table.len != 0)       hash_table_save(cmd[1], phash_table, fw);
    for     (int i = 1; i < footer_line_numbers; i++)                fputs(footer[i], fw);
    fclose(fw);

    return 0;
}
