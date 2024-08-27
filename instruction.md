# Instruction

## Prerequisites

Make sure you have the following dependencies installed:

- CMake
- Ninja
- Clang (version >= 16)
- LLVM (version >= 16)
- libzstd-dev
- libedit-dev
- uuid-dev
- Miniconda

You can install them on Debian or Ubuntu using the following command:

```bash
sudo apt install \
  make cmake ninja-build \
  clang-16 libclang-16-dev llvm-16 llvm-16-dev lld-16 \
  libzstd-dev libedit-dev uuid-dev

mkdir -p ~/miniconda3
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh -O ~/miniconda3/miniconda.sh
bash ~/miniconda3/miniconda.sh -b -u -p ~/miniconda3
rm -rf ~/miniconda3/miniconda.sh
```

## Installation

1. Create virtual environment:

```bash
conda create -n kernel -y
conda activate kernel
conda install xeus-zmq jupyter nlohmann_json -c conda-forge -y
```

2. Clone this repository:

```
git clone https:://github.com/wlonestar/llvm-project.git
cd llvm-project
git checkout repl
```

3. Configure and build:

```
./configure.sh
make
```

4. Install

```
make install
```

# Usage

## 1. Clang-Repl

Use Command `clang-repl` to enter interactive programming environment:

![](https://image-1305118058.cos.ap-nanjing.myqcloud.com/image/Snipaste_2024-06-03_08-47-45.jpg)

## 2. xeus-clang

Start using Jupyter Notebook or JupyterLab, and you will see C++ kernels:

![](https://image-1305118058.cos.ap-nanjing.myqcloud.com/image/Snipaste_2024-06-03_08-51-20.jpg)


