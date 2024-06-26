# 并发

## 线程安全问题的本质

主内存与工作内存中的数据不一致

## 线程同步

## 1. 互斥同步

**同步**：多个线程并发访问共享资源时，使数据在任何时刻只被一个线程访问。

互斥是同步的一种方法，通常基于互斥锁实现，是一种阻塞同步，除此之外还有非阻塞同步。

### 1.1 隐式锁

synchronized 就是一种隐式锁，通过 monitorenter 和 monitorexit 指令实现，在 JDK 1.6 之前，会直接调用 JVM 中 C++ 实现的管程对象，需要进行内核态的转换，十分耗费资源，属于重量级的锁。 1.6 中 JDK 采用了膨胀机制来対 synchronized 进行了优化。

### 1.2 显式锁

通过 Lock 进行显式的加解锁，最常用的一个实现是 ReentrantLock，多个线程调用 lock() 时，只有一个线程可以获得锁，其他线程就会阻塞在 lock() 上。需要注意的是为了避免异常而无法释放锁，必须把 unlock() 放在 finally 块中。

tryLock()：不管有没有获得锁都会返回一个 boolean 值，线程不会阻塞在这个方法上，所以当抢锁失败时可以自行指定重试策略，如随机休眠一段时间再重试：

```java
while(true){
    if(lock.tryLock()){
        try{
            // do something
        }finally{
            lock.unlock();
        }
    }

    Thread.sleep(random.nextInt(1000));
}
```

## 2. 非阻塞同步

锁会导致线程阻塞从而频繁地进行上下文切换和挂起，这些操作的时间可能已经超过了线程真正执行任务的时间，而现代处理器已经可以通过硬件实现复合操作的原子性。

**CompareAndSwap** 就是一种原子性的操作，这个操作有三个参数：

* 需要更新的变量 V（内存地址）
* 原值 A
* 新值 B

CAS 的过程就是**只有 V 的值等于 A 时，才将 B 赋给 V 并返回 true，否则直接返回 false**，所以多个线程基于 CAS 写某个变量时，只有一个线程会成功更新，其他线程都会被立刻告知更新失败，并不会阻塞，可以根据需要再次尝试。

应用：

* tryLock()：内部使用了 CAS 机制，只进行一次尝试，成功就将结果返回
* 原子类：如 AtomicInteger，它的 compareAndSet(int expect,int update) 就是调用了 native 的 CAS 方法

缺点：

* **ABA**：因为 CAS 操作会检查当前变量值和指定的值是否相同，即使值经过了 A-B-A 的变化也是无法察觉的。解决这个问题只需要在每次写入操作时增加一个版本号即可，Java 中 AtomicStampedReference 就提供了解决了 ABA 问题的原子操作
* **循环时间带来的开销**：在竞争非常激烈的情况下使用 CAS 会导致线程出现空循环的情况，浪费 CPU 资源，所以不如使用锁，这样其他竞争不到锁的线程至少会让出 CPU 资源
* **只能保证一个变量的原子性操作**：可以将多个变量放在对象中，AtomicReference 可以保证对象的原子操作

## 3. 锁

### 3.1 自旋锁

互斥同步中锁是通过阻塞线程来实现的，线程的阻塞和唤醒需要 CPU 进行切换上下文，有时候同步代码的执行时间比状态切换还短，这种情况下频繁地切换十分浪费资源，所以如果认为持有锁的线程能够在短时间内释放锁，那么准备竞争锁的其他线程完全可以不用被阻塞，而是「等待一会儿」，等待锁被释放即可。

自旋的原理就是 CAS 机制，在循环中通过 CAS 修改值，直至修改成功。所以**自旋是一种忙等待**，为了不令 CPU 长时间空转，一般需要一个最大自旋时间。

### 3.2 锁的可重入性

对于同一个线程，在外层方法获得锁后，运行内层需要获得锁的方法时不会因为之前没有释放锁而阻塞，可以一定程度上避免死锁。

synchronized 和 ReentrantLock 都是可重入锁，对于 ReentrantLock 的可重入性是由 AQS 中的 status 控制的：

* 线程获取锁时，可重入锁会更新 status，如果为 0 则表示没有其他线程执行同步代码，则将 status 置 1；如果不为 0，判断当前线程是否为获得锁的线程，如果是则 status + 1；而不可重入锁在判断 status 不为 0 后直接认为获取锁失败而阻塞
* 释放锁时，对于可重入锁，status - 1 == 0 则表示重入的锁已经释放完毕，则可以真正释放锁；而不可重入锁则直接将 status 置 0，释放锁

### 3.3 锁的公平性

#### 公平锁

多个线程按照申请锁的顺序获取锁，线程直接进入队列，只有第一个线程能够获取锁，其他线程都会阻塞
* 优点：线程不会饿死
* 缺点：吞吐量下降，因为需要维护队列，一个线程到达后，需要检查队首线程是否在等待锁，如果有则自己需要挂起，放到队尾并且唤醒队首线程

#### 非公平锁

多个线程竞争获取锁，不需要按照申请锁的顺序
* 优点：减少唤醒线程的消耗
* 缺点：线程可能饿死

### 3.4 锁的膨胀

为了减少获取锁和释放锁带来的性能消耗，JDK 6 的 synchronized 引入了「偏向锁」和「轻量级锁」
* 偏向锁：如果一段同步代码始终被同一个线程访问，则该线程会自动获取锁
    * 通过对比锁对象对象头中的数据来加锁，避免了 CAS 操作
* 轻量级锁：当遇到其他线程尝试竞争偏向锁时，偏向锁会升级为轻量级锁，此时其他线程会通过自旋的方式竞争锁，但不会阻塞
    * 通过 CAS 和自旋避免了线程阻塞和唤醒
* 重量级锁：当自旋超过一定次数或存在第三个线程竞争锁，则升级为重量级锁，会将持有锁以外的所有线程阻塞

## 4. 并发工具

### AQS

### CountDownLatch

两种使用场景：

* 计数为 1 的 CountDownLatch 阻塞所有线程，调用 countDown() 使计数归 0，所有线程同时开始执行

    ```java
    public static void main(String[] args) {

        CountDownLatch countDownLatch = new CountDownLatch(1);
        ExecutorService executorService = Executors.newFixedThreadPool(10);

        for (int i = 0; i < 10; i++) {
            int num = i;
            executorService.submit(() -> {
                try {
                    Thread.sleep(1000L);
                    // 线程阻塞
                    countDownLatch.await();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                System.out.println(num + " run");
            });
        }
        System.out.println("main run");
        // 主线程令所有线程同时开始
        countDownLatch.countDown();
    }
    ```

* 主线程阻塞，n 个线程对应计数器 n，每个线程执行完毕调用 countDown() 使计数器减 1，所有线程都执行结束主线程执行汇总

    ```java
    public static void main(String[] args) {

        CountDownLatch countDownLatch = new CountDownLatch(10);
        ExecutorService executorService = Executors.newFixedThreadPool(10);

        for (int i = 0; i < 10; i++) {
            int num = i;
            executorService.submit(() -> {
                try {
                    Thread.sleep(1000L);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                System.out.println(num + " completed");
                // 计数器减 1
                countDownLatch.countDown();
            });
        }

        try {
            // 主线程阻塞
            countDownLatch.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("main completed");
    }
    ```