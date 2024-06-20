# debugrik
Simple debugger for x86_64

# How to build
Install dependencies
```sh
conan install . --output-folder=build --build=missing
```
```
cd build
```
```sh
source conanbuild.sh
```
Configure build fiels
```sh
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
```
Build
```sh
cmake --build . --config Release
```

# Usage
## How to run
```sh
./debugrik <path/to/executable>
```

## Commands available
- `r` - run debugging program
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/f6202bb2-45a0-4f66-89e7-796e37c57fba)
- `b <addr>` - set break point on <addr>
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/255c7e79-97d1-4fcd-820a-197ca416d68b)
- `c` - continue execution
- `ir` - display registers values
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/1fe51682-010a-4fbd-8cbe-a87c8cae88cd)
- `s` - step one instruction
- `il` - display local variabels and its values
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/cfc8a5ca-e353-4e58-8bca-018838a5269c)
- `lf` - list all functions in binary
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/1ed1e5c0-c4b3-41c4-b0d7-5af39d62b0f2)
- `dis` - print disassembly listing of current function
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/aa78b3fa-ae6b-4246-82d2-854a1d189aee)
- `x <addr> <n>` - read <n> qwords of memory at the specified address <addr>
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/94c345e9-2078-4f65-a770-b054f9097f5d)
- `set <reg> <val>` - sets specified value - <val> for register - <reg>
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/7ae1ad10-020e-4dd3-bacb-82ef2b65a7d2)
- `n` - executes the next instruction and stops
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/27e21134-ee78-4209-9ab7-251fddab366f)
- `p <val>` - prints the value of the current instruction. 
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/e7b53a99-d8b5-47f2-ac6f-a928ba5a7fe7)
- `exit` - kill debugging target and exit
![изображение](https://github.com/devAL3X/debugrik/assets/40294005/47629370-09c4-40d5-a12f-cc5fc2865a0d)

## Run tests
```sh
cd ./build/
ctest
```
# For developers 
## Documentation
[Documentation](./docs/)

To generate documentation:
```
doxygen
```
