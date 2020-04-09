
if [ $# -eq "0" ]; then
    cd server
    ./build.sh debug rec
    cd client
    ./build.sh debug rec
elif [ $1 == "clean" ]; then
    cd server
    ./build.sh clean
    cd client
    ./build.sh clean
else
    echo "params error"
fi