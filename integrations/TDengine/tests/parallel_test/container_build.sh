#!/bin/bash
set -e 

function usage() {
    echo "$0"
    echo -e "\t -w work dir"
    echo -e "\t -e enterprise edition"
    echo -e "\t -b branch of taosadapter, default is main"
    echo -e "\t -h help"
}

ent=0
branch_taosadapter=main
while getopts "w:b:eh" opt; do
    case $opt in
        w)
            WORKDIR=$OPTARG
            ;;
        e)
            ent=1
            ;;
        b)  
            branch_taosadapter=$OPTARG
            ;;
        h)
            usage
            exit 0
            ;;
        \?)
            echo "Invalid option: -$OPTARG"
            usage
            exit 0
            ;;
    esac
done

if [ -z "$WORKDIR" ]; then
    usage
    exit 1
fi


ulimit -c unlimited

if [ $ent -eq 0 ]; then
    REP_DIR=/home/TDengine
    REP_REAL_PATH=$WORKDIR/TDengine
    REP_MOUNT_PARAM=$REP_REAL_PATH:/home/TDengine
else
    REP_DIR=/home/TDinternal
    REP_REAL_PATH=$WORKDIR/TDinternal
    REP_MOUNT_PARAM=$REP_REAL_PATH:/home/TDinternal
    
fi
date 
docker run \
    -v $REP_MOUNT_PARAM \
    -v /root/.cargo/registry:/root/.cargo/registry \
    -v /root/.cargo/git:/root/.cargo/git \
    -v /root/go/pkg/mod:/root/go/pkg/mod \
    -v /root/.cache/go-build:/root/.cache/go-build \
    -v /root/.cos-local.1:/root/.cos-local.2 \
    -v ${REP_REAL_PATH}/enterprise/contrib/grant-lib:${REP_DIR}/enterprise/contrib/grant-lib \
    -v ${REP_REAL_PATH}/community/tools/taosadapter:${REP_DIR}/community/tools/taosadapter \
    -v ${REP_REAL_PATH}/community/tools/taosws-rs:${REP_DIR}/community/tools/taosws-rs \
    -v ${REP_REAL_PATH}/community/contrib/apr/:${REP_DIR}/community/contrib/apr \
    -v ${REP_REAL_PATH}/community/contrib/apr-util/:${REP_DIR}/community/contrib/apr-util \
    -v ${REP_REAL_PATH}/community/contrib/cJson/:${REP_DIR}/community/contrib/cJson \
    -v ${REP_REAL_PATH}/community/contrib/cpp-stub/:${REP_DIR}/community/contrib/cpp-stub \
    -v ${REP_REAL_PATH}/community/contrib/curl/:${REP_DIR}/community/contrib/curl \
    -v ${REP_REAL_PATH}/community/contrib/curl2/:${REP_DIR}/community/contrib/curl2 \
    -v ${REP_REAL_PATH}/community/contrib/geos/:${REP_DIR}/community/contrib/geos \
    -v ${REP_REAL_PATH}/community/contrib/googletest/:${REP_DIR}/community/contrib/googletest \
    -v ${REP_REAL_PATH}/community/contrib/libs3/:${REP_DIR}/community/contrib/libs3 \
    -v ${REP_REAL_PATH}/community/contrib/libuv/:${REP_DIR}/community/contrib/libuv \
    -v ${REP_REAL_PATH}/community/contrib/lz4/:${REP_DIR}/community/contrib/lz4 \
    -v ${REP_REAL_PATH}/community/contrib/lzma2/:${REP_DIR}/community/contrib/lzma2 \
    -v ${REP_REAL_PATH}/community/contrib/mxml/:${REP_DIR}/community/contrib/mxml \
    -v ${REP_REAL_PATH}/community/contrib/openssl/:${REP_DIR}/community/contrib/openssl \
    -v ${REP_REAL_PATH}/community/contrib/pcre2/:${REP_DIR}/community/contrib/pcre2 \
    -v ${REP_REAL_PATH}/community/contrib/zlib/:${REP_DIR}/community/contrib/zlib \
    -v ${REP_REAL_PATH}/community/contrib/zstd/:${REP_DIR}/community/contrib/zstd \
    --rm --ulimit core=-1 taos_test:v1.0 sh -c "cd $REP_DIR; rm -rf debug; mkdir -p debug; cd debug; cmake .. -DBUILD_HTTP=internal -DBUILD_TOOLS=true -DBUILD_TEST=ON -DWEBSOCKET=true -DBUILD_TAOSX=false -DJEMALLOC_ENABLED=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DTAOSADAPTER_GIT_TAG:STRING=${branch_taosadapter} ;make -j|| exit 1"
 # -v ${REP_REAL_PATH}/community/contrib/jemalloc/:${REP_DIR}/community/contrib/jemalloc \

if [[ -d ${WORKDIR}/debugNoSan  ]] ;then
    echo "delete  ${WORKDIR}/debugNoSan"
    rm -rf  ${WORKDIR}/debugNoSan
fi
if [[ -d ${WORKDIR}/debugSan ]] ;then
    echo "delete  ${WORKDIR}/debugSan"
    rm -rf  ${WORKDIR}/debugSan
fi

mv  ${REP_REAL_PATH}/debug  ${WORKDIR}/debugNoSan
date
docker run \
    -v $REP_MOUNT_PARAM \
    -v /root/.cargo/registry:/root/.cargo/registry \
    -v /root/.cargo/git:/root/.cargo/git \
    -v /root/go/pkg/mod:/root/go/pkg/mod \
    -v /root/.cache/go-build:/root/.cache/go-build \
    -v /root/.cos-local.1:/root/.cos-local.2 \
    -v ${REP_REAL_PATH}/enterprise/contrib/grant-lib:${REP_DIR}/enterprise/contrib/grant-lib \
    -v ${REP_REAL_PATH}/community/tools/taosadapter:${REP_DIR}/community/tools/taosadapter \
    -v ${REP_REAL_PATH}/community/tools/taosws-rs:${REP_DIR}/community/tools/taosws-rs \
    -v ${REP_REAL_PATH}/community/tools/taosws-rs/target:${REP_DIR}/community/tools/taosws-rs/target \
    -v ${REP_REAL_PATH}/community/contrib/apr/:${REP_DIR}/community/contrib/apr \
    -v ${REP_REAL_PATH}/community/contrib/apr-util/:${REP_DIR}/community/contrib/apr-util \
    -v ${REP_REAL_PATH}/community/contrib/cJson/:${REP_DIR}/community/contrib/cJson \
    -v ${REP_REAL_PATH}/community/contrib/cpp-stub/:${REP_DIR}/community/contrib/cpp-stub \
    -v ${REP_REAL_PATH}/community/contrib/curl/:${REP_DIR}/community/contrib/curl \
    -v ${REP_REAL_PATH}/community/contrib/curl2/:${REP_DIR}/community/contrib/curl2 \
    -v ${REP_REAL_PATH}/community/contrib/geos/:${REP_DIR}/community/contrib/geos \
    -v ${REP_REAL_PATH}/community/contrib/googletest/:${REP_DIR}/community/contrib/googletest \
    -v ${REP_REAL_PATH}/community/contrib/libs3/:${REP_DIR}/community/contrib/libs3 \
    -v ${REP_REAL_PATH}/community/contrib/libuv/:${REP_DIR}/community/contrib/libuv \
    -v ${REP_REAL_PATH}/community/contrib/lz4/:${REP_DIR}/community/contrib/lz4 \
    -v ${REP_REAL_PATH}/community/contrib/lzma2/:${REP_DIR}/community/contrib/lzma2 \
    -v ${REP_REAL_PATH}/community/contrib/mxml/:${REP_DIR}/community/contrib/mxml \
    -v ${REP_REAL_PATH}/community/contrib/openssl/:${REP_DIR}/community/contrib/openssl \
    -v ${REP_REAL_PATH}/community/contrib/pcre2/:${REP_DIR}/community/contrib/pcre2 \
    -v ${REP_REAL_PATH}/community/contrib/zlib/:${REP_DIR}/community/contrib/zlib \
    -v ${REP_REAL_PATH}/community/contrib/zstd/:${REP_DIR}/community/contrib/zstd \
    --rm --ulimit core=-1 taos_test:v1.0 sh -c "cd $REP_DIR; rm -rf debug; mkdir -p debug; cd debug; cmake .. -DBUILD_HTTP=internal -DBUILD_TOOLS=true -DBUILD_TEST=ON -DWEBSOCKET=true -DBUILD_SANITIZER=1 -DTOOLS_SANITIZE=true -DCMAKE_BUILD_TYPE=Debug -DTOOLS_BUILD_TYPE=Debug -DBUILD_TAOSX=false -DJEMALLOC_ENABLED=OFF -DTAOSADAPTER_GIT_TAG:STRING=${branch_taosadapter} ; make -j|| exit 1 "

mv  ${REP_REAL_PATH}/debug  ${WORKDIR}/debugSan

date



ret=$?
exit $ret

