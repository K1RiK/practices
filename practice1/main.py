from sys import argv


def main():

    SET_COMMANDS = ("SADD", "SREM", "SISMEMBER")
    STACK_COMMANDS = ("SPUSH", "SPOP", "SPOP")
    QUEUE_COMMANDS = ("QPUSH", "QPOP", "QPOP")
    HASH_TABLE_COMMANDS = ("HSET", "HDEL", "HGET")

    if '--file' in argv:
        file = argv.index('--file') + 1
    else:
        file = -1

    if '--query' in argv:
        cmd = argv.index('--query') + 1
    else:
        cmd = -1

    if file >= len(argv) or file < 0:
        print('Укажите флаг --file <имя файла>')
        return 1

    if cmd >= len(argv) or cmd < 0:
        print('Укажите флаг --query <запрос>')
        return 1

    file = argv[file]
    cmd = argv[cmd::]

    if cmd[0] in SET_COMMANDS:
        work = 'set'
        _set = set()
    elif cmd[0] in HASH_TABLE_COMMANDS:
        work = 'hash'
        _hash_table = dict()
    elif cmd[0] in STACK_COMMANDS:
        work = 'stack'
        _stack = []
    elif cmd[0] in QUEUE_COMMANDS:
        work = 'queue'
        _queue = []
    else:
        print(f"Ошибка запроса! Нет такой команды {cmd[0]}")

    with open(file, 'r+', encoding='utf-8') as f:
        head = ""
        footer = []
        lines = f.readlines()

        for line in lines:
            split_line = line.strip().split(' ')
            if split_line[0] == work and split_line[1] == cmd[1]:
                if work == 'set':
                    _set = set(split_line[2::])
                elif work == 'hash':
                    for el in split_line[2::]:
                        key, value = el.split(':')
                        _hash_table[key] = value
                elif work == 'stack':
                    _stack = split_line[2::]
                elif work == 'queue':
                    _queue = split_line[2::]
                footer += lines[lines.index(line)::]
                break    
            head += line
            line = f.readline()

        if cmd[0] == 'SADD':
            _set.add(cmd[2])
        elif cmd[0] == 'SREM':
            try:
                _set.remove(cmd[2])
            except KeyError:
                print(f"Такого элемента нет в множестве {cmd[1]}")
        elif cmd[0] == 'SISMEMBER':
            print(cmd[2] in _set)

        elif cmd[0] == 'HSET':
            _hash_table[cmd[2]] = cmd[3]
        elif cmd[0] == 'HDEL':
            _hash_table.pop(cmd[2])
        elif cmd[0] == 'HGET':
            print(cmd[2] in _hash_table.keys())

        elif cmd[0] == 'SPUSH':
            _stack.append(cmd[2])
        elif cmd[0] == 'SPOP':
            print("POPPED:", _stack[-1])
            _stack.pop(-1)

        elif cmd[0] == 'QPUSH':
            _queue.append(cmd[2])
        elif cmd[0] == 'QPOP':
            print("POPPED QUEUE:", _queue[0])
            _queue.pop(0)

    with open(file, 'w+', encoding='utf-8') as f:
        if work == 'set':
            text = f'{work} {cmd[1]} {" ".join(_set)}\n' if _set else ''
        elif work == 'stack':
            text = f'{work} {cmd[1]} {" ".join(_stack)}\n' if _stack else ''
        elif work == 'hash':
            include = [f'{key}:{value}' for key, value in _hash_table.items()]
            text = f'{work} {cmd[1]} {" ".join(include)}\n' if include else ''
        elif work == 'queue':
            text = f'{work} {cmd[1]} {" ".join(_queue)}\n' if _queue else ''
        footer = ''.join(footer[1::])
        f.write(head + text + footer)


if __name__ == '__main__':
    main()
