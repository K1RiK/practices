#define set_lenght 512

typedef struct {
    char data[set_lenght][128];
    unsigned int len;
    int (*hash)(char*);
} Set;

void sadd(Set* set, char value[]) {  // Добавляем значение в множество
    int hash = set->hash(value);
    snprintf(set->data[hash], sizeof(set->data[hash]), "%s", value);
    set->len++;
}

int srem(Set* set, char value[]) {  // Удаляем значение из множества
    if (set->len == 0) return -1;
    int hash = set->hash(value);
    if (strcmp(set->data[hash], "") != 0) {
        snprintf(set->data[hash], sizeof(set->data[hash]), "");
        set->len--;
        return hash;
    }
    return -1;
}

int sismember(Set* set, char value[]) {  // Проверяем есть ли значение в множестве
    return strcmp(set->data[set->hash(value)], "") != 0 ? 1 : 0;
}

void set_save(char* name, Set* set, FILE* file) {  // Функция для сохранения множества в файл
    char data[1024];
    snprintf(data, sizeof(data), "set %s", name);
    for (int i = 0, c = 0; i < set_lenght && c < set->len; i++) {
        if (strcmp(set->data[i], "") != 0) {
            snprintf(data, sizeof(data), "%s %s", data, set->data[i]);
            c++;
        }
    }
    snprintf(data, sizeof(data), "%s\n", data);
    printf("\nSET DATA:%s", data);
    fputs(data, file);
}
