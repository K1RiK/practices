typedef struct {
  HANDLE mutex;
  SOCKET socket;
} mutex_t;

mutex_t* mutex_init(SOCKET socket) {
  mutex_t* mutex = calloc(1, sizeof(mutex_t));
  mutex->mutex = CreateMutex(NULL, FALSE, NULL);
  if (mutex->mutex == NULL) {
    free(mutex);
    return NULL;
  }
  mutex->socket = socket;
  return mutex;
}

void mutex_lock(mutex_t *mutex) {
  WaitForSingleObject(mutex->mutex, INFINITE);
}

void mutex_unlock(mutex_t *mutex) {
  ReleaseMutex(mutex->mutex);
}

void mutex_destroy(mutex_t *mutex) {
  CloseHandle(mutex->mutex);
  free(mutex);
}
