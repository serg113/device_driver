1. build character device driver \
$ make

2. add module to the kernel \
$ make load \
or \
$ sudo insmod copy-char-dev-module.ko 

3. remove module \
$ make unload \
or \
$ sudo rmmod copy-char-dev-module.ko 


*The character device driver registers itself at /dev/copy 


<b> -- delete module -- </b> \
sudo rmmod copy-char-dev-module.ko

<b> -- print all device messages in console -- </b> \
cat /dev/kmsg



<b> references: </b>

1. https://linux-kernel-labs.github.io/refs/heads/master/labs/device_drivers.html

2. https://linux-kernel-labs.github.io/refs/heads/master/labs/filesystems_part1.html

3. https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module

4. https://www.linuxquestions.org/questions/programming-9/character-device-mounting-799077/

5. https://web.archive.org/web/20131208033328/http://www.fiveanddime.net:80/man-pages/kmalloc.9.html

6. https://www.linuxquestions.org/questions/programming-9/character-device-mounting-799077/

