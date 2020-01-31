int num_pipes = num_cmds - 1;
int fds[num_pipes][2];
int pid;
int first_child_pid;
int stdin_cpy = dup(0);
int stdout_cpy = dup(1);

for(int i = 0; i < num_pipes; i++) {
  const int pipe_status = pipe(&fds[i][0]);
  if (pipe_status != 0) {
    printf("An error occured during pipe creation\n");
  }
}

for (int i = 0; i < num_cmds; i++) {
  char *cmd_name = argv[commands[i]];
  // char *in_path = *(argv + in_redir[i]);
  // char *out_path = *(argv + out_redir[i]);
  char *env[] = { NULL };
  if ((pid = fork()) == 0 ) {
    if (!first_child_pid) {
      first_child_pid = getpid();
    }
    setpgid(getpid(), first_child_pid);
    printf("Executing %s\n", cmd_name);

    if ( i < num_cmds - 1) {
      dup2(fds[i][1], 1);
    }
    if (i > 0) {
      dup2(fds[i - 1][0], 0);
    }
    for (int j = 0; j < num_pipes; j++) {
      close(fds[j][0]);
      close(fds[j][1]);
    }
    execve(argv[commands[i]], &argv[commands[i]], env);
    exit(0);
  } else {
    if (i == 0) {
      first_child_pid = pid;
    }
    int child_status;
    close(fds[i][1]);
    if (i > 0) {
      close(fds[i - 1][0]);
    }
    if (i == num_cmds - 1) {
      dup2(stdin_cpy, 0);
      dup2(stdout_cpy, 1);
      for (int j = 0; j < num_pipes; j++) {
        close(fds[j][0]);
        close(fds[j][1]);
      }
    }
    // printf("parent\n");
    wait(&child_status);
    // printf("PASSED HERE\n");
  }
}
return;

}
