# 阻塞 IO

```
listenfd = socket();
bind(listenfd);
listen(listenfd);

while(1) {
  connfd = accept(listenfd);  // 阻塞建立连接
  int n = read(connfd, buf);  // 阻塞读数据
  doSomeThing(buf);
  close(connfd);
}
```
## 不足

阻塞在两个阶段：

* accept：等待三次握手
* read：等待数据从网卡到内核缓冲区再将数据从内核缓冲区拷贝到用户缓冲区

# 非阻塞 IO

## Trick

```
while(1) {
  connfd = accept(listenfd);  // 阻塞建立连接
  pthread_create(doWork);  // 创建一个新的线程
}

void doWork() {
  int n = read(connfd, buf);  // 阻塞读数据
  doSomeThing(buf); 
  close(connfd);
}
```

## 不足

通过多线程的方式将 read 的阻塞从主线程中剥离，read 不再阻塞 accetp，但 read 本身仍然是阻塞的

## 非阻塞

OS 提供非阻塞的 read：在数据还未到达内核缓冲区时，调用可以直接返回而不是原地等待

```
while(1) {
  connfd = accept(listenfd);  // 阻塞建立连接
  pthread_create(doWork);  // 创建一个新的线程
}

void doWork() {
  while(1) {
	  if(read(connfd, buf)!=-1){  // 非阻塞读数据
		  doSomeThing(buf); 
	  }
  }
}
```

## 不足

为每个客户端分配一个线程十分占用服务器资源

# 多路复用 IO

## Trick

```
while(1) {  
  connfd = accept(listenfd);  // 阻塞建立连接
  fdlist.add(connfd);  // 将连接放入一个数组
}

while(1) {  
  for(fd <-- fdlist) {  
    if(read(fd) != -1) {  
      doSomeThing();  
    }  
  }  
}
```

## 不足

在上一个模型的基础上，只用了一个线程就完成了服务端和多个客户端的 IO，但循环进行系统调用的方式依然会频繁地进行用户态和内核态的切换

## 多路复用器（select）

OS 提供多路复用函数：将遍历 read 的操作封装进系统调用中，避免了用户态和内核态的切换

![](image/非阻塞%20IO-多路复用.jpg)

```
while(1) {
  connfd = accept(listenfd);  // 阻塞建立连接
  fdlist.add(connfd);  // 将连接放入一个数组
}

while(1) {
  // 阻塞，等待 OS 监听到有 fd 已就绪读
  nready = select(list);  
  // 用户层依然要遍历，只不过少了很多无效的系统调用  
  for(fd <-- fdlist) {  
    if(fd != -1) {  
      // 只读已就绪的文件描述符  
      read(fd, buf);   
    }
    // 总共只有 nready 个已就绪描述符，不用过多遍历  
    if(--nready == 0){
	  break;
    }
  }
}
```

可以看到多路复用器 select 实现了用户态下的对多个连接进行遍历的过程，将发生变化的文件描述符进行特殊标记，表示 Socket 可读或者可写，从而可以进行读写操作，减少了一部分非必要的上下文切换

需要注意的是：多路复用指的是使用一个线程管理多个连接的 IO（感知读写事件），而不是只使用一个线程处理多个连接的读写逻辑（可以使用一个，也可以使用线程池）

## 不足

* 基本的多路复用器 select 一次最多只能监听 1024 个 fd
* 将 fd 传给 select 后，这 1024 个数据需要拷贝至内核，存在一定的资源消耗
* select 的结果只是 fd ready 的数量，并不知道哪些 fd ready 了

## 其他多路复用器

### poll

与 select 类似，不同的是改为使用链表的方式维护 fd，不再有大小限制

### epoll

* 通过 epoll_create 创建 epoll 对象
* 通过 epoll_ctl 向其中加入 fd（通过红黑树维护 fd）
* 通过 epoll_wait 获取 ready 的 fd

省去了拷贝 fd 的过程，并且可以直接处理 ready 的 fd

```
int s = socket(AF_INET, SOCK_STREAM, 0);
bind(s, ...);
listen(s, ...)

int epfd = epoll_create(...);
epoll_ctl(epfd, ...);  //将所有需要监听的socket添加到epfd中

while(1) {
	// 阻塞，等待 fd 就绪
    int n = epoll_wait(...);
    for(接收到数据的socket){
        //处理
    }
}
```