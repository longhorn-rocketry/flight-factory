if [ "$#" -ne 2 ]; then
    echo "Usage: ./fly.sh <path to sketch> <path to config file>"
    exit 1
fi

./build.sh $1
./ff $2
