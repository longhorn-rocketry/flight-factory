if [ "$#" -ne 1 ]; then
    echo "Usage: ./fly.sh <path to sketch>"
    exit 1
fi

./build.sh $1
./ff $1
