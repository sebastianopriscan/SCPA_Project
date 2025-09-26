#! /bin/bash -i

if [[ ! -f ./.ssh-exec.config ]]; then
    echo 'Please create .ssh-exec.config file with SSH user and password'
fi

. .ssh-exec.config

while [[ $# -gt 0 ]]; do
    if [[ $1 == '--' ]]; then
        shift
        break
    fi

    ssh ${SSH_EXEC_USER} "mkdir -p $(dirname ${SSH_EXEC_TARGET_DIR}${1})"
    scp $1 "${SSH_EXEC_USER}:${SSH_EXEC_TARGET_DIR}${1}" > /dev/null 2> /dev/null
    shift
done

ssh ${SSH_EXEC_USER} "cd ${SSH_EXEC_TARGET_DIR} && $@"
