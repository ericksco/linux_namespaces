#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
 * demonstrates use of setns() to join a namespace. argv[1] should be a namespace path such as /proc/<pid>/ns/pid. Note that for some namespaces, such as pid, this program will not switch to the namespace
 * right away. Some namespaces, such as PID, require a clone() (new pid) to be created -- run the below and execute any command to enter the namespace in that case (e.g. `join /proc/$$/ns/pid /bin/bash`,
 * then execute `/bin/bash`.
 */

int main(int argc, char **argv)
{
	int fd;

	fd = open(argv[1], O_RDONLY);   /* Get descriptor for namespace */
	setns(fd, 0);                   /* Join that namespace */
	execvp(argv[2], &argv[2]);      /* Execute a command in namespace */

	return 33;
}
