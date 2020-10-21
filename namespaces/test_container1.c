#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>

/*
 *	This process unshares UTS and PID namespaces, then forks a child. The unshare immediately affects the parent, placing it into the UTS namespace but the PID namespace isn't created until
 *	the exec is called, placing the child within it. 
 *
 *	After the fork, the child changes it's UTS name and goes to sleep.
 *
 * 	Note that this new PID namespace still accesses the same NETNS namespace (mount), so querying /proc will display parts of the parent's PID namespace as the info is available via vfs. 
 * 	Mounting /proc explicitly will show reality.
 *
 *	Running `nsenter -t <childpid> -u -p /bin/bash` will spawn a bash shell in this new namespace for exploration purposes.
 */


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
        	printf("I am parent, my pid is: %d\n", getpid());
		wait(&y);
        }
	else	/* Child */
	{
        	printf("I am child, my pid is: %d\n", getpid());

		/* set hostname */
		sethostname("container", 10);

		/* change to chroot jail so we can mount a new proc that reflects our new pid namespace */
		mkdir("/mnt/controot", 700);
		chroot("/mnt/controot");
		chdir("/mnt/controot");

		mount("dummy", "/proc", "proc", 0, 0);

		sleep(600);

		// execve(argv[1],argv+1,0);

		exit(0);
	}

        return 0;
}
