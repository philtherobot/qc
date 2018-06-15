#!/usr/bin/env bash

shopt -s nullglob


function err
{
    echo error: ${*}
    exit 1
}
export -f err


function CHECK_EQUAL
{
    if [[ "${1}" != "${2}" ]]; then err ${3} ; fi
}
export -f CHECK_EQUAL


function CHECK_EXIT_STATUS_OK
{
    if [[ ${?} -ne 0 ]]; then err exit status non-zero; fi
}
export -f CHECK_EXIT_STATUS_OK


function CHECK_EXIT_STATUS
{
    if [[ ${?} -ne ${1} ]]; then err exit status is not ${1}; fi
}
export -f CHECK_EXIT_STATUS


# We compile/modify/compile program in quick sequence
# and sometimes the filesystem records the same file
# modification time.
function delay_for_fs
{
    sleep 1.0s
}
export -f delay_for_fs


nbtests=0
nbfailures=0


for d in *
do
    if [[ -e "${d}/test.sh" ]]
    then
        let nbtests++ 
        echo -n Running ${d}...

        (
            cd "${d}"
            ./test.sh
        )

        if [[ ${?} -eq 0 ]]
        then
            echo OK
        else
            echo !!!! Failed !!!!
            let nbfailures++
        fi
    fi
done

echo
echo Ran ${nbtests} tests.
echo There were ${nbfailures} failures.


# TODO
# include dependencies OK (one and more levels down)
# include file with a space
# running from dir A a program in dir B which includes relative to itself