# 线程

---

## 1. 线程的实现

Java 中 Thread 类中的关键方法均为 native 方法，说明 Java 底层对线程的实现是平台相关的，借助操作系统的线程实现，Java 提供的是不同平台下对线程的统一处理。Windows 与 Linux 使用的是一对一模型实现线程，JVM 中直接使用了这种方式。

## 2. 线程池

![Executor Class](img/executor_class.png)

线程池状态：

* RUNNING：线程池被创建后处于此状态
* SHUTDOWN：调用 shutDown() 后，此时线程池不能接受新任务，但会等待所有任务执行完毕
* STOP：调用 shutDownNow() 后，此时线程池不能接受新的任务，并尝试中断正在执行的任务，清空任务队列，返回尚未执行的任务
* TERMINATED：线程池处于 SHUTDOWN / STOP 并且所有工作线程已经销毁，任务缓存队列已经清空或执行结束

**任务执行机制**：

* 当前线程数目小于 corePoolSize 时，每来一个任务就会创建一个线程执行任务
* 当前线程数目大于等于 corePoolSize 时，每来一个任务则会尝试将其添加到任务队列中
  * 若添加成功，则会等待空闲线程将其取出并执行
  * 若添加失败，则会尝试创建新的线程去执行任务
* 对于空闲时间超过 keepAliveTime 的线程将会被中止，直至池中线程数目不大于 corePoolSize
* 当前线程数达到 maximumPoolSize 时则会采取任务拒绝策略进行处理

**工作队列**：

* 无界队列：如 LinkedBlockingQueue 默认容量是最大的 int 值，这样池中的线程数目永远不会达到 maximumPoolSize，适用于每个任务独立互不影响的情况，但需要注意任务提交速度，防止过度积压占用内存
* 有界队列：如 ArrayBlockingQueue，当队列满后，后续提交的任务根据拒绝策略处理
* 同步移交队列：如 SynchronousQueue，默认工作队列，底层没有链表或数组，也就是说没有存储空间，一旦有任务到达，没有线程处理就会阻塞，所以  maximumPoolSize 会设置得较大；同时，没有额外的数据结构使得性能比其他队列要好

拒绝策略：

* AbortPolicy：默认策略，拒绝新提交的任务，并抛出异常
* CallerRunsPolicy：令提交任务的线程执行任务，不向线程池中添加
* DiscardPolicy：丢弃任务
* DiscardOldestPolicy：丢弃最旧的未处理任务

**创建线程池**：

```java
ExecutorService service = new ThreadPoolExecutor(args)
```

args：

* corePoolSize：核心线程数
* maximumPoolSize：最大线程数
* keepAliveTime：空闲时间
* unit：空闲时间单位
* workQueue：工作队列
* threadFactory：线程工厂，用来设置线程池中线程的一些属性
* handler：拒绝策略

**预定义线程池**：
  
```java
public static ExecutorService newFixedThreadPool(int nThreads) {
    return new ThreadPoolExecutor(nThreads, nThreads,
                                    0L, TimeUnit.MILLISECONDS,
                                    new LinkedBlockingQueue<Runnable>());
}
```

固定数量的线程 + 无界队列

```java
public static ExecutorService newSingleThreadExecutor() {
    return new FinalizableDelegatedExecutorService
        (new ThreadPoolExecutor(1, 1,
                                0L, TimeUnit.MILLISECONDS,
                                new LinkedBlockingQueue<Runnable>()));
}
```

只能使用 1 个线程，顺序执行

```java
public static ExecutorService newCachedThreadPool() {
    return new ThreadPoolExecutor(0, Integer.MAX_VALUE,
                                    60L, TimeUnit.SECONDS,
                                    new SynchronousQueue<Runnable>());
}
```

任务到达就立即执行，线程数可以无限扩大

线程数量设置：

* CPU 密集型程序：增加线程不会有明显的提高因为 CPU 的利用率始终很高，一般设置为**处理器数量 + 1**，多一个为了在某个线程暂停时切换到这个线程继续执行，最大限度利用 CPU
* IO 密集型程序：处理器数量 * 2

## 3. 线程的生命周期

![thread state](img/thread_state.png)

* NEW：仅仅是 JVM 为其分配了内存，初始化变量
* RUNNABLE：创建栈和程序计数器，等待被 CPU 调度
* RUNNING：获得了 CPU 的时间片，处于执行状态
* WAITING / TIMEWAITING：进入锁对象的等待队列中，会释放 CPU 资源，需要其他线程唤醒或到达设定时间
* BLOCKED：线程因竞争不到锁而产生阻塞，会释放 CPU 资源，只有获得锁才会脱离阻塞态
* TERMINATED：线程执行完毕或出现异常

需要注意的是，BLOCKED 阻塞是因为竞争不到锁资源而被迫停止，而 WAITING 和 TIME WAITING 则仅仅是自行产生阻塞，和锁无关。这三种阻塞仅仅定义在 Java 层面，而操作系统中只有一种阻塞状态，所以 sleep 能够「不释放锁但释放 CPU 资源」

> OS 层面阻塞的本质：将进程挂起，不再参与进程调度，即 OS 调度器不会再将 CPU 时间片分给此进程

### 3.1 start / run

start() 是线程的启动方法，会判断线程是否处于 NEW 状态，然后将线程添加到 ThreadGroup 中，并调用 native 的 start0()，start0() 会开启线程的特性，等待操作系统调度。

run() 只是一个对象的普通方法，因为会在 start0() 中被调用所以线程才会执行它，如果直接调用就和其他方法一样只会顺序执行

> 需要注意的是，线程池底层都是使用了 run()

### 3.2 wait / notify / sleep

当一个线程获取锁后如果任务执行的条件还不满足，则应该主动让出锁，去等待区等待，直到其他线程完成执行条件，再通知等待区的线程去执行任务

### 先获取锁

wait 和 notify 的线程都是关联在锁对象上的，所以需要先获取锁后才能执行这些操作

### wai

当执行了 wait 后线程会被阻塞，释放锁和 CPU 资源，当其他线程唤醒了这个等待线程后，才从 wait 返回，也就是从 wait 之后的地方继续执行

### notif

* notify 不会释放锁
* 它会唤醒锁对象上的其他线程，让这些线程去重新竞争锁

 > 需要注意的是，notify 对线程的唤醒是指让它们从 OS 的阻塞态中恢复，变成「可被分配 CPU 时间片」的状态

### sleep

sleep 不会释放锁，但会释放 CPU 资源

> 由 sleep 的这种机制可以看出，锁是由 JVM 实现的，而线程的阻塞是 JVM 借用了 OS 的实现，二者没有必然的联系

### 3.4 中断

正确的中断线程的方式应该是在线程内部检查该线程的中断标记，但是阻塞的线程无法主动地检测中断标记，所以引入了线程的中断状态：

* 默认为 false，即未中断
* Java 提供了获取或操作中断状态的方法

wait()、sleep() 等造成阻塞期间会检测当前线程的中断状态，如果为 true 就退出阻塞、抛出 InterruptedException，所以可以通过捕获中断异常来中止阻塞中的线程

## 4. ThreadLocal

ThreadLocal 解决了线程内共享变量的问题，它存储的变量在线程间隔离，每个线程都可以有这个变量独立的副本。

### 4.1 实现原理

* ThreadLocal 存在一个静态内部类 ThreadLocalMap，以 ThreadLocal 对象作为 key 进行值的存放

> 为什么用 TL 作为 key？
> 因为一个线程可能会有多个 TL 对象

* Thread 为每个线程维护了一个 ThreadLocalMap 的并持有它的引用，这样在线程中访问 ThreadLocal 的时候就可以通过当前线程对象获取到 ThreadLocalMap，再根据当前 ThreadLocal 作为key 从而获取到值

    ```java
    // Thread
    ThreadLocal.ThreadLocalMap threadLocals = null;
    ```

**set()**：

```java
public void set(T value) {
    Thread t = Thread.currentThread();
    ThreadLocalMap map = getMap(t);
    if (map != null)
        map.set(this, value);
    else
        createMap(t, value);
}
```

```java
ThreadLocalMap getMap(Thread t) {
    return t.threadLocals;
}
```

```java
void createMap(Thread t, T firstValue) {
    t.threadLocals = new ThreadLocalMap(this, firstValue);
}
```

可以看到 set() 中先获取到了当前线程对象，通过线程对象拿到了 Map 的引用：

* 如果 Map 已存在，则以当前 ThreadLocal 对象为 key 存放 value
* 如果当前线程的 Map 未创建，则实例化一个 Map 赋值给线程对象的 threadLocals

**get()**：

```java
public T get() {
    Thread t = Thread.currentThread();
    ThreadLocalMap map = getMap(t);
    if (map != null) {
        ThreadLocalMap.Entry e = map.getEntry(this);
        if (e != null) {
            @SuppressWarnings("unchecked")
            T result = (T)e.value;
            return result;
        }
    }
    return setInitialValue();
}
```

与 set() 类似，获取到 Map 的引用，根据当前 ThreadLocal 对象获取值。

### 4.2 内存泄漏

可能导致内存泄漏的是 TL 在堆上创建的 TLM 中的 key、value

![threadlocal ref](img/threadlocal.png)

#### 情况一

栈上的 TL 引用不再被使用，但还被 TLM 持有，导致 TL 无法被 GC

* 解决：Entry 持有 ThreadLocal 对象的弱引用，当线程中不再使用 ThreadLocal 对应的 value 后，只剩弱引用的 key 就可以被 GC，防止内存泄漏

```java
static class Entry extends WeakReference<ThreadLocal<?>> {  
    /** The value associated with this ThreadLocal. */  
    Object value;  
  
    Entry(ThreadLocal<?> k, Object v) {  
        super(k);  
        value = v;  
    }  
}
```

#### 情况二

只有 Thread 对象一直存在（线程池），那么 TLM 中的 value 对象就一直被持有，无法被 GC

* 解决：在调用 TL 的 get() / set() / remove() 的时候都会清除所有 key 为 null 的 value，所以在 TL 用完后手动调用 TL 的 remove() 即可

### 4.3 应用

Spring 中 TL 的应用：
* 将 db 连接绑定到当前线程来保证这个线程中对 db 的操作用的是同一个连接就是将 db 连接放在了 TL 中