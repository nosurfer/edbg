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
- [x] kill process with exit
- [ ] other architectures?

### Done
- [x] ptrace_fork func
- [x] add logs (extern lib)
- [x] temporary makefile
