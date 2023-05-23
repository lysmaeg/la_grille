if [ -z $(command -v cmake) ]; then
    echo "Please install cmake with 'sudo apt install cmake'"
    exit
fi

if [ -z $(command -v make) ];then
    echo "Please install make with 'sudo apt install make'"
    exit
fi

mkdir build
cd build
cmake ..
make
cd ..
