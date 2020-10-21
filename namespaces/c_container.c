#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>

int main(int argc, char **argv)
{
        int unshareflags = CLONE_NEWUTS | CLONE_NEWPID;
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
		wait(&y);
        }
	else	/* Child */
	{
        	printf("I am child, my pid is: %d\n", getpid());

		/* set hostname */
		sethostname("container", 10);

		/* change root */
		if (chroot("/mnt/container") != 0)
		{
			perror("chroot");
		}

		/* change directory to new root */
		if (chdir("/") != 0)
		{
			perror("chdir");
		}

		/* mount up a new proc */
		mount("dummy", "/proc", "proc", 0, 0);

		z = fork();
		if (z != 0)
		{
			/* parent -- wait for child, then cleanup */
			wait(&y);
			umount("/proc");
		}
		else
		{
			/* execute passed in command */
			execv(argv[1], argv+1);
			/* shouldn't get here */
			exit(1);
		}

		exit(0);
	}

        return 0;
}
