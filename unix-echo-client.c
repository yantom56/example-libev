/////////////////////
#if 0
17:05:29 T1 user@localhost:~/tmp/unixsocket $ gcc unix-echo-client.c -o unix-echo-client -lev -Wall -W -O2 -g
unix-echo-client.c: In function 'stdin_cb':
unix-echo-client.c:69:7: warning: variable 'len2' set but not used [-Wunused-but-set-variable]
   int len2; // not sure if this is at all useful
       ^
unix-echo-client.c: At top level:
unix-echo-client.c:25:13: warning: 'send_cb' defined but not used [-Wunused-function]
 static void send_cb (EV_P_ ev_io *w, int revents)
             ^
unix-echo-client.c:67:13: warning: 'stdin_cb' defined but not used [-Wunused-function]
 static void stdin_cb (EV_P_ ev_io *w, int revents)
             ^
unix-echo-client.c:78:13: warning: 'remote_cb' defined but not used [-Wunused-function]
 static void remote_cb (EV_P_ ev_io *w, int revents)
             ^
16:33:03 T1 user@localhost:~/tmp/unixsocket $ ./unix-echo-client
1533141188......... 1
1533141190......... 100001
1533141193......... 200001
1533141195......... 300001
1533141197......... 400001
1533141198......... 500001
1533141200......... 600001
1533141202......... 700001
^C
16:33:22 T1 user@localhost:~/tmp/unixsocket $ 
#endif


#include <stdlib.h>
#include <stdio.h>
#include <ev.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <sys/fcntl.h> // fcntl
#include <unistd.h> // close
#include <time.h>

// TODO
// disconnect and reconnect on each newline

// Nasty globals for now
// feel free to fork this example and clean it up
EV_P;
ev_io stdin_watcher;
ev_io remote_w;
ev_io send_w;
int remote_fd;
char* line = NULL;
size_t len = 0;
#if 0
static void send_cb (EV_P_ ev_io *w, int revents)
{
  if (revents & EV_WRITE)
  {
    puts ("remote ready for writing...");

    if (-1 == send(remote_fd, line, len, 0)) {
      perror("echo send");
      exit(EXIT_FAILURE);
    }
    // once the data is sent, stop notifications that
    // data can be sent until there is actually more 
    // data to send
    ev_io_stop(EV_A_ &send_w);
    ev_io_set(&send_w, remote_fd, EV_READ);
    ev_io_start(EV_A_ &send_w);
  }
  else if (revents & EV_READ)
  {
    int n;
    char str[100] = ".\0";

    printf("[r,remote]");
    n = recv(remote_fd, str, 100, 0);
    if (n <= 0) {
      if (0 == n) {
        // an orderly disconnect
        puts("orderly disconnect");
        ev_io_stop(EV_A_ &send_w);
        close(remote_fd);
      }  else if (EAGAIN == errno) {
        puts("should never get in this state with libev");
      } else {
        perror("recv");
      }
      return;
    }
    printf("socket client said: %s", str);

  }
}

static void stdin_cb (EV_P_ ev_io *w, int revents)
{
  int len2; // not sure if this is at all useful

  puts ("stdin written to, reading...");
  len2 = getline(&line, &len, stdin);
  ev_io_stop(EV_A_ &send_w);
  ev_io_set (&send_w, remote_fd, EV_READ | EV_WRITE);
  ev_io_start(EV_A_ &send_w);
}

static void remote_cb (EV_P_ ev_io *w, int revents)
{
  puts ("connected, now watching stdin");
  // Once the connection is established, listen to stdin
  ev_io_start(EV_A_ &stdin_watcher);
  // Once we're connected, that's the end of that
  ev_io_stop(EV_A_ &remote_w);
}


// Simply adds O_NONBLOCK to the file descriptor of choice
int setnonblock(int fd)
{
  int flags;
return 0;
  flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}
#endif
static void connection_new(char* sock_path) {
  int len;
  struct sockaddr_un remote;

  if (-1 == (remote_fd = socket(AF_UNIX, SOCK_STREAM, 0))) {
      perror("socket");
      exit(1);
  }
#if 0
  // Set it non-blocking
  if (-1 == setnonblock(remote_fd)) {
    perror("echo client socket nonblock");
    exit(EXIT_FAILURE);
  }

  // this should be initialized before the connect() so
  // that no packets are dropped when initially sent?
  // http://cr.yp.to/docs/connect.html

  // initialize the connect callback so that it starts the stdin asap
  ev_io_init (&remote_w, remote_cb, remote_fd, EV_WRITE);
  ev_io_start(EV_A_ &remote_w);
  // initialize the send callback, but wait to start until there is data to write
  ev_io_init(&send_w, send_cb, remote_fd, EV_READ);
  ev_io_start(EV_A_ &send_w);
#endif
  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, sock_path);
  len = strlen(remote.sun_path) + sizeof(remote.sun_family);

  if (-1 == connect(remote_fd, (struct sockaddr *)&remote, len)) {
      perror("connect");
      //exit(1);
  }
}

int main (void)
{
#if 0
  loop = EV_DEFAULT;
  // initialise an io watcher, then start it
  // this one will watch for stdin to become readable
  setnonblock(0);
  ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
#endif
  connection_new("/tmp/libev-echo.sock");
#if 0
  // now wait for events to arrive
  ev_loop(EV_A_ 0);
#else
  char lineddd[256];
  line = lineddd;
  int i = 0;
  while (1) {
  if (-1 == send(remote_fd, line, 128, 0)) {
	perror("echo send");
	exit(EXIT_FAILURE);
  }
  {
    int n;
    char str[256] = ".\0";

//    printf("[r,remote]");
    n = recv(remote_fd, str, 256, 0);
    if (n <= 0) {
      if (0 == n) {
        // an orderly disconnect
        puts("orderly disconnect");
        ev_io_stop(EV_A_ &send_w);
        close(remote_fd);
      }  else if (EAGAIN == errno) {
        puts("should never get in this state with libev");
      } else {
        perror("recv");
      }
      return -1;
    }
//    printf("socket client said: %s\n", str);
  }
  ///
  if (++i % 100000 == 0)
	  printf("%ld......... %d\n", time(NULL), i);
  }
#endif
  // unloop was called, so exit
  return 0;
}

