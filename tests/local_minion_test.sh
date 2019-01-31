#!/bin/bash
source ../conf/master.conf

cd ..

echo "Testing vdr for nbd number $devicePath"

sudo modprobe nbd

if ! make vdr minion
then 
	echo
	echo "Failed making master and minion"
	exit
fi

for N in {0..15}
do
echo 4 | sudo tee /sys/block/nbd$N/queue/max_sectors_kb 1> /dev/null
done

sudo ./minion.out&
sleep 1

sudo ./vdr.out&

sleep 3
if ! sudo mkfs.ext4 $devicePath
then 
	echo "Failed creating filesystem"
	exit
fi

if ! sudo mount $devicePath /mnt
then
	echo
	echo "Failed mounting device to filesystem"
	exit
fi

echo OK | sudo tee /mnt/test_file 1> /dev/null
cat /mnt/test_file > tmp_result
sudo umount -l /mnt
sudo nbd-client -d $devicePath
echo
echo
cat tmp_result
rm tmp_result
pkill minion.out