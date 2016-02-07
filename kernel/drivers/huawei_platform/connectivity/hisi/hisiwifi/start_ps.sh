insmod /data/wifi.ko loglevel=3
sleep 3
echo 1 > /sys/hi110x/powermgmt_switch
echo 6 > /sys/hi110x/watchdog_timeout

netcfg wlan0 up
echo "wlan0 up..."

ifconfig wlan0 192.168.1.110

iw wlan0 scan ssid xxoo  
iw wlan0 connect xxoo

echo "ping 5 pkt..."
ping -c 5 192.168.1.1

