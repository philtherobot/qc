#!/usr/bin/env bash

shopt -s nullglob


function error_and_exit
{
    echo error: ${*}
    exit 1
}
export -f error_and_exit


function check_failed
{
    echo failure: ${*}
    exit 2
}
export -f check_failed


function CHECK_OUTPUT
{
    if [[ "${1}" != "${2}" ]]
    then 
        check_failed script output is not what is expected \(\"${1}\" != \"${2}\"\)
    fi
}
export -f CHECK_OUTPUT


function CHECK_EXIT_STATUS_OK
{
    if [[ ${?} -ne 0 ]]
    then
        check_failed exit status non-zero
    fi
}
export -f CHECK_EXIT_STATUS_OK


function CHECK_EXIT_STATUS
{
    if [[ ${?} -ne ${1} ]]
    then
        check_failed exit status is not ${1}
    fi
}
export -f CHECK_EXIT_STATUS


# We compile/modify/compile scripts in quick sequence
# and sometimes the filesystem records the same file
# modification time.  Use this delay to workaround
# the problem.
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
# running from dir A a script in dir B which includes relative to itself
