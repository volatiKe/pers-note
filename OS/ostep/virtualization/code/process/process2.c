#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    printf("hello world (pid:%d)\n", (int)getpid());

    int rc = fork();

    if(rc < 0){
        fprintf(stderr, "fork failed\n");
    }else if(rc == 0){
        printf("hello, I am child (pid:%d)\n", (int)getpid());
    }else{
        /**
         * wait()会暂时停止目前进程的执行, 直到有信号来到或子进程结束. 如果在调用wait()时子进程已经结束, 则wait()会立即返回子进程结束状态值. 子进程的结束状态值会由参数status 返回, 而子进程的进程识别码也会一快返回. 如果不在意结束状态值, 则参数 status 可以设成NULL
         */
        int rc_wait = wait(NULL);
        printf("hello, I am parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int)getpid());
    }

    return 0;
}