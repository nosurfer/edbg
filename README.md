# edbg

## Install
```bash
git clone https://github.com/nosurfer/edbg.git
cd edbg
make pull
make
./edbg
```

## Todo
- [ ] more comments everywhere... (needed?)
- [ ] getting registers info
  - [x] configuration (wrapper)
- [ ] continue handler
  - [x] configuration (wrapper)
- [ ] process status handling
  - [x] handler
- [ ] step handler
  - [x] configuration (wrapper)
- [ ] info maps (vmmap)
  - [x] handler
- [ ] memmory read / write
  - [ ] configuration (wrapper)
- [ ] disassemble
  - [x] handler
- [ ] breakpoint handler
  - [ ] configuration (wrapper)
- [ ] next handler
  - [ ] configuration (wrapper)
- [ ] help and quit features
- [ ] tests

## Thoughts
- [ ] kill process with exit
- [ ] other architectures?

### Done
- [x] ptrace_fork func
- [x] add logs (extern lib)
- [x] temporary makefile


### Улучшенный интерфейс командной строки
- **Автодополнение по Tab** – введите начало команды, нажмите Tab.
- **История команд** – стрелки ↑/↓.
- **Парсер опций** – короткие опции (`-la`), длинные (`--help`, `--count=5`), значения через `=` или пробел.
- **Глобальный `--help`** – любая команда с `--help` показывает справку.
- **Новая команда `help`** – выводит список команд или справку по конкретной команде.
- **Команда `attach PID`** – теперь доступна в REPL.