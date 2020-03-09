
if [ $# -eq "0" ]; then
    cd server
    ./build.sh debug rec
elif [ $1 == "clean" ]; then
    cd server
    ./build.sh clean
else
    echo "params error"
fi