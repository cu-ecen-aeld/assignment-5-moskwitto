#!/bin/bash

set -e

case "$1" in
    start)
        echo "Starting aesdsocket in daemon mode"
        ./aesdsocket --start --quiet --background --exec /path/to/aesdsocket -- -d
        ;;
    stop)
        echo "Stopping aesdsocket"
        ./aesdsocket --stop --signal SIGTERM --quiet --retry 5 --exec /path/to/aesdsocket -- -d
        ;;
    restart)
        "$0" stop
        sleep 1
        "$0" start
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
esac

exit 0

