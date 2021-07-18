1. build character device driver 
    $ make

2. add module to the kernel
    $ sudo insmod copy-char-dev-module.ko

3. check major number from log
    $ cat /dev/kmsg
    or list all devices to find new added one
    $ cat /proc/devices

4. add node to the /dev, in my case major number was 238
    $ sudo mknod /dev/copy c <major_number> 0

5. for testing
    $ sudo su
    $ cat "any_file.ext" > /dev/copy
    // after this step /dev/output will be created
    $ sudo mv /dev/output .
    $ sudo chmod 664 output
    $ sudo chown "user":"user" output
    $ mv output "any_file.ext"





//delete module
sudo rmmod copy-char-dev-module.ko

// to print all device messages in console
cat /dev/kmsg

// to write simple binary file
echo -n $'\x01' > binary.dat
echo -n $'\x02' >> binary.dat


references:
1. https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html

2. https://linux-kernel-labs.github.io/refs/heads/master/labs/filesystems_part1.html

3. https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module

4. https://www.unix.com/man-page/suse/2/open/

5. https://www.linuxquestions.org/questions/programming-9/character-device-mounting-799077/

6.https://web.archive.org/web/20131208033328/http://www.fiveanddime.net:80/man-pages/kmalloc.9.html

7. https://www.linuxquestions.org/questions/programming-9/character-device-mounting-799077/

