#!/system/bin/sh

log_print() {
  echo $1
  log -p i -t launch_daemonsu "$1"
}

loopsetup() {
  LOOPDEVICE=
  for DEV in $(ls /dev/block/loop*); do
    LS=$(losetup $DEV $1 2>/dev/null)
    if [ $? -eq 0 ]; then
      LOOPDEVICE=$DEV
      break
    fi
  done
}

remount_ro() {
  TMP=$(cat /proc/self/mountinfo | grep "$1 rw")
  if [ $? -eq 0 ]; then
    mount -o ro,remount $1
  fi
}

if [ ! -d "/su/bin" ]; then
  # if we fstab'd system/vendor/oem to rw, remount them ro here
  remount_ro /system
  remount_ro /vendor
  remount_ro /oem

  # not mounted yet, do our thing
  REBOOT=false

  # move boot image backups
  mv /cache/stock_boot_* /data/.
  rm /cache/stock_boot_*

  # newer image in /cache ?
  # only used if recovery couldn't mount /data
  if [ -f "/cache/su.img" ]; then
    log_print "/cache/su.img found"

    OVERWRITE=true

    if [ -f "/data/su.img" ]; then
      # attempt merge, this will fail pre-M
      # will probably also fail with /system installed busybox,
      # but then again, is there anything busybox doesn't break?
      # falls back to overwrite

      log_print "/data/su.img found"
      log_print "attempting merge"

      mkdir /cache/data_img
      mkdir /cache/cache_img

      # setup loop devices

      loopsetup /data/su.img
      LOOPDATA=$LOOPDEVICE
      log_print "$LOOPDATA /data/su.img"

      loopsetup /cache/su.img
      LOOPCACHE=$LOOPDEVICE
      log_print "$LOOPCACHE /cache/su.img"

      if [ ! -z "$LOOPDATA" ]; then
        if [ ! -z "$LOOPCACHE" ]; then
          # if loop devices have been setup, mount images
          OK=true

          mount -t ext4 -o rw,noatime $LOOPDATA /cache/data_img
          if [ $? -ne 0 ]; then
            OK=false
          fi

          mount -t ext4 -o rw,noatime $LOOPCACHE /cache/cache_img
          if [ $? -ne 0 ]; then
            OK=false
          fi

          if ($OK); then
            # if mounts have succeeded, merge the images
            cp -af /cache/cache_img/. /cache/data_img
            if [ $? -eq 0 ]; then
              log_print "merge complete"
              OVERWRITE=false
            fi
          fi

          umount /cache/data_img
          umount /cache/cache_img
        fi
      fi

      rmdir /cache/data_img
      rmdir /cache/cache_img
    fi

    if ($OVERWRITE); then
      # no /data/su.img or merge failed, replace
      log_print "replacing /data/su.img with /cache/su.img"
      mv /cache/su.img /data/su.img
    fi

    rm /cache/su.img
  fi

  # do we have an APK to install ?
  if [ -f "/cache/SuperSU.apk" ]; then
    cp /cache/SuperSU.apk /data/SuperSU.apk
    rm /cache/SuperSU.apk
  fi
  if [ -f "/data/SuperSU.apk" ]; then
    log_print "installing SuperSU APK in /data"

    APKPATH=eu.chainfire.supersu-1
    for i in `ls /data/app | grep eu.chainfire.supersu | grep -v eu.chainfire.supersu.pro`; do
      APKPATH=$i
      rm -rf /data/app/$APKPATH
      break;
    done

    log_print "target path: /data/app/$APKPATH"

    mkdir /data/app/$APKPATH
    chown 1000.1000 /data/app/$APKPATH
    chmod 0755 /data/app/$APKPATH
    chcon u:object_r:apk_data_file:s0 /data/app/$APKPATH

    cp /data/SuperSU.apk /data/app/$APKPATH/base.apk
    chown 1000.1000 /data/app/$APKPATH/base.apk
    chmod 0644 /data/app/$APKPATH/base.apk
    chcon u:object_r:apk_data_file:s0 /data/app/$APKPATH/base.apk

    rm /data/SuperSU.apk

    # just in case
    REBOOT=true
  fi

  # sometimes we need to reboot, make it so
  if ($REBOOT); then
    log_print "rebooting"
    reboot
  fi

  # trigger mount, also works pre-M
  chcon u:object_r:system_data_file:s0 /data/su.img
  chmod 0600 /data/su.img
  setprop sukernel.mount 1
  sleep 1

  # if other su binaries exist, route them to ours
  mount -o bind /su/bin/su /sbin/su
  mount -o bind /su/bin/su /system/bin/su
  mount -o bind /su/bin/su /system/xbin/su

  # poor man's overlay on /system/xbin
  if [ -d "/su/xbin_bind" ]; then
    cp -f -a /system/xbin/. /su/xbin_bind
    rm -rf /su/xbin_bind/su
    ln -s /su/bin/su /su/xbin_bind/su
    mount -o bind /su/xbin_bind /system/xbin
  fi
fi

# start daemon
exec /su/bin/daemonsu --auto-daemon
