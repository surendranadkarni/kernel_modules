#ifndef CHARDEV_H
#define CHARDEV_H
#include <linux/ioctl.h>
/*
 * * The major device number. We can't rely on dynamic
 * * registration any more, because ioctls need to know
 * * it.
 * */
#define MAJOR_NUM 100
/*
 * * Set the message of the device driver
 * */
#define IOCTL_SET_MSG _IOW(MAJOR_NUM, 0, char *)
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)
#define IOCTL_GET_NTH_BYTE _IOWR(MAJOR_NUM, 2, int)
#define DEVICE_FILE_NAME "char_dev"
#endif
