# Object detection for video surveillance

`det` file is an ugly version. For the final version see `motdet`.

It has two modes: 'background substraction' and 'basic'.

Background substraction (mode 1) uses, well, background substraction available in openCV and saves only one picture of an 'intruder'.

The basic method (mode 0) compares 'background' image and the current image and if the difference belongs to the set interval, saves the current image as an image with an intruder. The background image is updated regularly to minimase noise from clouds/light changes etc.

Usable on mini-computers

## Building

Prerequisites: CMake, OpenCV

```
mkdir build
cmake ..
make
```

## Testing

```
./det
```

```
./motdet
```

