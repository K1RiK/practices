#define hash_table_lenght 512

typedef struct {
    char keys[hash_table_lenght][256];
    char values[hash_table_lenght][256];
    unsigned int len;
    int (*hash)(char*);
} Hash_table;

void hset(Hash_table* hash_table, char* key, char* value) {  // Добавляем ключ и значение в таблицу
    int hash = hash_table->hash(key);
    if (strcmp(hash_table->keys[hash], key) == 0) {
        snprintf(hash_table->values[hash], sizeof(hash_table->values[hash]), "%s", value);
        hash = hash_table_lenght;
    }
    for (; hash < hash_table_lenght; hash++) {
        if (strcmp(hash_table->keys[hash], "") == 0) {
            snprintf(hash_table->keys[hash], sizeof(hash_table->keys[hash]), "%s", key);
            snprintf(hash_table->values[hash], sizeof(hash_table->values[hash]), "%s", value);
            break;
        }
    }
    hash_table->len++;
}

void hdel(Hash_table* hash_table, char* key) {  // Удаляем ключ и значение из таблицу
    if (hash_table->len != 0) {
        int hash = hash_table->hash(key);
        for (; hash < hash_table_lenght; hash++) {
            if (strcmp(hash_table->keys[hash], key) == 0) {
                snprintf(hash_table->keys[hash], sizeof(hash_table->keys[hash]), "");
                snprintf(hash_table->values[hash], sizeof(hash_table->values[hash]), "");
                hash_table->len--;
                break;
            }
        }
    }
}

char* hget(Hash_table* hash_table, char* key) {  // Получаем значение по ключу
    if (hash_table->len != 0) {
        int hash = hash_table->hash(key);
        for (; hash < hash_table_lenght; hash++) {
            if (strcmp(hash_table->keys[hash], key) == 0) {
                return hash_table->values[hash];
            }
        }
    }
    return NULL;
}

void hash_table_save (char* name, Hash_table* hash_table, FILE* file) {  // Функция для сохранения таблицы в файл
    char data[4096];
    snprintf(data, sizeof(data), "hash %s", name);
    for (int i = 0, c = 0; i < hash_table_lenght && c < hash_table->len; i++) {
        if (strcmp(hash_table->keys[i], "") != 0) {
            snprintf(data, sizeof(data), "%s %s:%s", data, hash_table->keys[i], hash_table->values[i]);
            c++;
        }
    }
    snprintf(data, sizeof(data), "%s\n", data);
    printf("HASH DATA:%s\n", data);
    fputs(data, file);
}
