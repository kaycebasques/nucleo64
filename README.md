# nucleo64

[Nucleo-64]: https://www.st.com/en/evaluation-tools/nucleo-f401re.html

Bare-metal apps for the [Nucleo-64] (STM32F401RE).

## Background

[Bare Metal Programming Series]: https://youtu.be/uQQsDWLRDuI
[lowbyteproductions/bare-metal-series]: https://github.com/lowbyteproductions/bare-metal-series

This code is based off the [Bare Metal Programming Series] by Low Byte
Productions. The original repo is [lowbyteproductions/bare-metal-series]. My
repo is a bit different than the original repo in a few ways:

* **Simplified Makefile**. TODO
* **Single-file apps**. TODO
* **Self-contained build environment**. TODO

## Setup

```
git clone --recursive git@github.com:kaycebasques/nucleo64.git
```

## Activate the development environment

```
source libs/pigweed/activate.sh
```

## Build

```
make
```

## Flash

```
make flash
```
