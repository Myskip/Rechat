#!/bin/bash

################### configs #################
SOCKET_UTILS_DIR=~/codes/socket_utils
CWD=$PWD
SRC_DIR=$CWD/src
BUILD_DIR=$CWD/build

#############################################
CFLAG="-I$SOCKET_UTILS_DIR/include"
RELEASE="release"
DEBUG="debug"
INC="inc"
REC="rec"
DBG=0
EXE=server

function err_message()
{
    echo "Args $1 invalid..."
    echo "Uage: ./build.sh [$RELEASE|$DEBUG|clean] [$INC|$REC]"
    echo '$1:'
    echo "release: release version"
    echo "debug: debug version"
    echo "clean: clean the project"
    echo '$2:'
    echo "inc: increment compile"
    echo "rec: recompile"
}

function usage()
{
    if [ $# -eq 1 ]; then
        if test $1 != "clean"; then
            err_message 1
            exit -1;
        fi 
    elif [ $# -eq 2 ];then
        if test $1 != $DEBUG -a $1 != $RELEASE; then
            err_message 1
            exit -1
        fi

        if test $2 != $INC -a $2 != $REC -a -n $2; then
            err_message 2
            exit -1
        fi
    else
        err_message 1
        exit -1
    fi
}

function compile_project()
{
    if [ $1 == $REC ]; then
        clean_project
    fi

    make CFLAG=$CFLAG DBG=$DBG 
    make -C $SOCKET_UTILS_DIR DBG=$DBG
    make -C $SOCKET_UTILS_DIR lib
}

function gather_objects()
{
    if [ -e build ]; then
        echo "build exist"
        rm -rf $BUILD_DIR/*.*
    else
        echo "build not exist"
        mkdir build
    fi

    cp -r $SRC_DIR/*.o $BUILD_DIR/
    cp -r $SOCKET_UTILS_DIR/*.a $BUILD_DIR/ 
}

function generate_program()
{
    cc -o $EXE $BUILD_DIR/*.o $BUILD_DIR/*.a -lpthread
}

function clean_project()
{
    make clean
    make -C $SOCKET_UTILS_DIR clean
    rm -r $BUILD_DIR/*.*
    rm $EXE
}

usage $@

if [ $1 == $DEBUG ]; then
    DBG=1 
elif [ $1 == $RELEASE ]; then
    DBG=0
elif [ $1 == "clean" ]; then
    clean_project
    exit 1
fi

cd $CWD

compile_project $2
gather_objects
generate_program

exit 1
