
## About
K-means clustering simulation using the Lloyd–Forgy algorithm.
---
## Compilation and Execution
The dataset is contained in the `input.txt` file. To compile and run the application, execute the following commands in the root directory:
```bash
gcc $( pkg-config --cflags gtk4 epoxy ) app/src/*.c -o kmeans $( pkg-config --libs gtk4 epoxy ) -lm
./kmeans
```
---
## Directory Structure
```
root/
├── app/
│   └── src/
│       └── *.c
├── input.txt
└── README.md
```
