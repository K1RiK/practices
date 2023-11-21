typedef struct {
    char data[64][128];
    unsigned int head;
} Stack;

void spush(Stack* stack, char value[]) {  // Заносим значение в стек
    snprintf(stack->data[stack->head], sizeof(stack->data[stack->head]), "%s", value);
    stack->head++;
}

char* spop(Stack* stack) {  // Достаем значение из стека
    return stack->head != 0 ? stack->data[--stack->head] : NULL;
}

void stack_save(char* name, Stack* stack, FILE* file) {  // Функция для сохранения стека в файл
    char data[1024];
    snprintf(data, sizeof(data), "stack %s", name);
    for (int i = 0; i != stack->head; i++)
        snprintf(data, sizeof(data), "%s %s", data, stack->data[i]);
    snprintf(data, sizeof(data), "%s\n", data);
    printf("\nSTACK DATA:%s", data);
    fputs(data, file);
}
