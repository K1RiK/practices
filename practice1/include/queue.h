typedef struct {
    char data[64][128];
    unsigned int start, end;
} Queue;

void qpush(Queue* queue, char value[]) {  // Добавляем значение в очередь
    snprintf(queue->data[queue->end], sizeof(queue->data[queue->end]), "%s", value);
    queue->end++;
}

char* qpop(Queue* queue) {  // Удаляем элемент из очерди
    return (queue->end != queue->start) ? queue->data[queue->start++] : NULL;
}

void queue_save(char* name, Queue* queue, FILE* file) {  // Функция для сохранения очереди в файл
    char data[1024];
    snprintf(data, sizeof(data), "queue %s", name);
    for (int i = queue->start; i != queue->end; i++)
        snprintf(data, sizeof(data), "%s %s", data, queue->data[i]);
    snprintf(data, sizeof(data), "%s\n", data);
    printf("\nQUEUE DATA:%s", data);
    fputs(data, file);
}
