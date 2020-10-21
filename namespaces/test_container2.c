#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/mount.h>

/*
 *	This process unshares UTS, PID and mount namespaces, then forks a child, sets up some bind mounts for a chroot jail and moves to it. CLONE_NEWNS starts the child with a copy of the parent's mount space, so we just bind mount the already-mounted filesystems. 
 *
 * 	This process also creates a new network namespace, so an `/sbin/ip link ls` only shows "lo"
 *
 *
 */


int main(int argc, char **argv)
{
        int unshareflags = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET;
        int x, y, z = 0;

	/* create new namespaces */
        if ((x = unshare(unshareflags)) < 0)
        {
                perror("unshare");
        }

	/* fork a child in those new namespaces */
        x = fork();
        if (x != 0)
        {
		/* parent */
        	printf("I am parent, my pid is: %d\n", getpid());
		wait(&y);

		/* cleanup the mounts and mountpoints we left behind */
		umount("/mnt/controot/proc");
		umount("/mnt/controot/var");
		umount("/mnt/controot/");
		rmdir("/mnt/controot/");

        }
	else	/* Child */
	{
        	printf("I am child, my pid is: %d\n", getpid());

		/* set hostname */
		sethostname("container", 10);

		/* change to chroot jail so we can mount a new proc that reflects our new pid namespace */
		mkdir("/mnt/controot", 700);

		/* we need some basic filesystems to run bash in our new namespace */
		mount("/", "/mnt/controot/", "xfs", MS_BIND, 0);
		mount("/var", "/mnt/controot/var", "xfs", MS_BIND, 0);
		mount("dummy", "/mnt/controot/proc", "proc", 0, 0);

		chroot("/mnt/controot");
		execve(argv[1], argv+1, 0);

		exit(0);
	}

        return 0;
}
