#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Usage: sudo ./test [NbdNumber]    (e.g: './test 0')" >&2
  exit 1
fi

echo "Testing for nbd number $1"

make clean
make
echo 4 | sudo tee /sys/block/nbd$1/queue/max_sectors_kb
sudo ./a.out /dev/nbd$1 10000&
sleep 3
sudo mkfs.ext4 /dev/nbd$1
sudo mount /dev/nbd$1 /mnt
echo OK | sudo tee /mnt/test_file
cat /mnt/test_file > result
sudo umount -l /mnt
sudo nbd-client -d /dev/nbd$1
echo
echo
cat result