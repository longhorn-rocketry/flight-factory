if [ "$#" -ne 1 ]; then
    echo "Usage: ./build.sh <path to sketch>"
    exit 1
fi

python3 make.py $1
make clean
make ff
