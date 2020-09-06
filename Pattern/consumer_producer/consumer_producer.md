# 生产者-消费者模式

---

## 1. 基于线程通信

共享资源

```java
private static Integer count = 0;
private static final int FULL = 10;
private static final int EMPTY = 0;
private static final String LOCK = "LOCK";
```

生产者

```java
class Producer implements Runnable {

    @Override
    public void run() {
        while (true) {
            synchronized (LOCK) {
                while (count == FULL) {
                    try {
                        LOCK.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                // 上边的 while 就是等待线程的通用模式，以下则是通知线程的通用模式
                count++;
                System.out.println("生产者" + Thread.currentThread().getName() + "进行了生产，目前数量：" + count);
                LOCK.notifyAll();
            }
        }
    }
}
```

消费者

```java
class Consumer implements Runnable {

    @Override
    public void run() {
        // while(true) 使得生产者和消费者不断交替执行
        while (true){
            synchronized (LOCK) {
                while (count == EMPTY) {
                    try {
                        LOCK.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                count--;
                System.out.println("消费者" + Thread.currentThread().getName() + "进行了消费，目前数量：" + count);
                LOCK.notifyAll();
            }
        }
    }
}
```

基于 wait / nofity 的生产者消费者模式就是将等待线程和通知线程的通用模式组合了起来。

## 2. 基于显式锁

共享资源

```java
private static Integer count = 0;
private static final int FULL = 10;
private static final int EMPTY = 0;
private Lock lock = new ReentrantLock();
private Condition notFull = lock.newCondition();
private Condition notEmpty = lock.newCondition();
```

生产者

```java
class Producer implements Runnable {

    @Override
    public void run() {
        while (true) {
            lock.lock();
            try {
                while (count == FULL) {
                    notFull.await();
                }
                count++;
                System.out.println("生产者" + Thread.currentThread().getName() + "进行了生产，目前数量：" + count);
                notEmpty.signalAll();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                lock.unlock();
            }
        }
    }
}
```

消费者

```java
class Consumer implements Runnable {

    @Override
    public void run() {
        while (true) {
            lock.lock();
            try {
                while (count == EMPTY) {
                    notEmpty.await();
                }
                count--;
                System.out.println("消费者" + Thread.currentThread().getName() + "进行了消费，目前数量：" + count);
                notFull.signalAll();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                lock.unlock();
            }
        }
    }
}
```

与内置锁的生产者消费者类似，显式锁下也是将基于 Condition 的等待线程和通知线程的通过模式结合在了一起。

## 3. 基于阻塞队列

共享资源

```java
private BlockingQueue<Integer> queue = new LinkedBlockingQueue<>(10);
```

生产者

```java
class Producer implements Runnable {

    @Override
    public void run() {
        while (true) {
            try {
                queue.put(1);
                System.out.println("生产者" + Thread.currentThread().getName() + "进行了生产，目前数量：" + queue.size());
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
```

消费者

```java
class Consumer implements Runnable {

    @Override
    public void run() {
        while (true) {
            try {
                queue.take();
                System.out.println("消费者" + Thread.currentThread().getName() + "进行了消费，目前数量：" + queue.size());
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
    }
}
```

需要注意的是需要使用队列的阻塞方法：put() / take()

## 4. 阻塞队列的基本实现

```java

public class BlockQueue {

    private LinkedList<Object> list = new LinkedList<Object>();
    private Object LOCK = new Object();
    private final int min = 0;
    private final int max;
    private int size = 0;


    BlockQueueComplete(int size) {
        max = size;
    }

    public void put(Object obj){
        synchronized(LOCK){
            while(size == max){
                LOCK.wait();
            }
            list.add(obj);
            size++;
            LOCK.notifyAll();
        }
    }

    public Object take(){
        synchronized(LOCK){
            while(size == min){
                LOCK.wait();
            }
            Object obj = list.removeFirst();
            size--;
            LOCK.notifyAll();
        }
    }
}
