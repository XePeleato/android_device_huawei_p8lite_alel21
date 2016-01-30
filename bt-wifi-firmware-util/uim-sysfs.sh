#! /bin/sh

uim=/usr/sbin/uim

test -x "$uim" || exit 0

case "$1" in
  start)
    echo -n "Starting uim-sysfs daemon"
    start-stop-daemon --start --quiet --exec $uim &
    echo "."
    ;;
  stop)
    echo -n "Stopping uim-sysfs daemon"
    start-stop-daemon --stop --quiet --pidfile /var/run/uim.pid
    echo "."
    ;;
  *)
    echo "Usage: /etc/init.d/uim-sysfs.sh {start|stop}"
    exit 1
esac

exit 0

