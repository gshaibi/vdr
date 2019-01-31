#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: sudo ./test [NbdNumber]    (e.g: './test 0')" >&2
  exit 1
fi

cd ..

echo "Testing vdr for nbd number $1"

sudo modprobe nbd
make vdr
make minion
echo 4 | sudo tee /sys/block/nbd$1/queue/max_sectors_kb 1> /dev/null
sudo ./minion.out 1 2&
sleep 1
sudo ./vdr.out /dev/nbd$1 10000&
sleep 1
sudo mkfs.ext4 /dev/nbd$1

if ! sudo mount /dev/nbd$1 /mnt
then
	echo
	echo "FAIL"
	exit
fi

echo OK | sudo tee /mnt/test_file 1> /dev/null
cat /mnt/test_file > tmp_result
sudo umount -l /mnt
sudo nbd-client -d /dev/nbd$1
echo
echo
cat tmp_result
rm tmp_result