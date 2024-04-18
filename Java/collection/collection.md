## Collection

### List

#### ArrayList

##### 数组拷贝

System.arraycopy()：

```java
public static native void arraycopy(Object src,
                                    int srcPos,
                                    Object dest,
                                    int destPos,
                                    int length);
```

这是一个 native 方法，能够从 src 中 srcPos 位置开始将 length 长度的元素复制到 dest 的 destPos 位置。

Arrays.copyOf()：

```java
public static <T,U> T[] copyOf(U[] original, int newLength, Class<? extends T[]> newType) {
    @SuppressWarnings("unchecked")
    T[] copy = ((Object)newType == (Object)Object[].class)
        ? (T[]) new Object[newLength]
        : (T[]) Array.newInstance(newType.getComponentType(), newLength);
    System.arraycopy(original, 0, copy, 0, Math.min(original.length, newLength));
    return copy;
}
```

可以看到 copyOf() 是在内部创建了一个新的数组，然后调用 System.arraycopy() 进行元素的复制并返回新数组。需要注意的是，copyOf() 对于引用类型数据仅仅拷贝地址，也就是说它是属于**浅拷贝**。

##### 扩容

ArrayList 会扩容到当前的 1.5 倍容量，使用 Arrays.copyOf() 进行数组复制操作。

##### 插入

* 指定位置：需要移动元素，所以时间复杂度为 O(n-i)，移动元素也是通过 Arrays.copyOf() 实现
* 不指定位置：直接增加至末尾，时间复杂度为 O(1)，存储大量数据时效率可能会超过 LinkedList

可以发现，ArrayList 的消耗较大的就是扩容和在指定位置插入元素，所以使用中最好事先指定大小，避免频繁的扩容。

#### LinkedList

LinkedList 的底层是一个双向链表。链表的查询效率较低，所以在查询时会和容器的 size 比较，如果查找位置靠近头部，就从链表头开始查找，否则就从链表尾部查找，时间复杂度是 O(n/2)，但是比顺序查找快不了多少。

### Set

#### HashSet

```java
public boolean add(E e){
    return map.put(e, PRESENT) == null;
}
```

HashSet 的底层就是 HashMap，HashSet 内部定义一个 PRESENT 对象。当添加元素时会将这个对象作为 value 插入 HashMap，这样如果 key 存在，则由于 value 相同则 key 不会发生任何变化；如果 key 不存在，那么元素成功插入。

### Queue / Deque

Deque 支持同时从两端添加或移除元素，属于双端队列。因此 Deque 接口的实现可以被当作 FIFO 队列使用，也可以当作 LIFO 队列（栈）来使用。

ArrayDeque 是 Deque 的一种实现：

* 底层依赖于可变数组
* 没有容量限制，可自动进行扩容
* ArrayDeque 不支持值为 null 的元素
* 因为是线程不安全的，所以相比于 Stack 性能更好

线程安全的队列：

* 阻塞队列：BlockingQueue
* 非阻塞队列：ConcurrentLinkedQueue

### Map

#### HashMap

##### Q & A

为什么 String、Integer 这种包装类更适合作为键？

* 这些包装类都是 final 修饰而不可继承的，而且内部实际存值的属性也是 final 的，它们作为键是不可变的
* 都重写了 equal() 和 hashCode()，不会出现 put 和 get 时 hash 不同的情况

JDK 8 的 HashMap 为什么使用红黑树而不是普通的二叉树？

* 二叉树存在退化成链表的可能性，而红黑树是一种平衡树，查找的平均效率相比二叉树更高

##### HashMap 的线程安全问题

* 1.7 下并发 put 触发并发扩容时，会导致链表成环，1.8 修复了这个问题
* 并发 put 可能会导致值覆盖的问题，这是因为容器本身不是线程安全的
* put 时并发 get，如果 put 触发扩容，那么 get 可能拿不到值

#### ConcurrentHashMap

* put 时如果某个位置为空，则 cas，否则加锁再 put，同时支持数组长度个线程并发操作容器
* 扩容时其他线程 put，如何保证不丢数据
	* 正在扩容会去协助扩容，而不是阻塞住，扩容完毕再 put
* 扩容时其他线程 get，通过老下标去新数组里可能拿不到数据
	* 单个位置的数据转移过程中不修改原始位置数据
	* 单个位置的数据转移完毕后，此位置指向新位置
	* 所有位置的数据转移完毕后，直接容器指向新数组

#### LinkedHashMap

LinkedHashMap 继承自 HashMap，但在其基础上，按照插入元素的顺序，将所有元素使用双向链表的方式连接起来，并维护有头尾指针。

并且，LinkedHashMap 的结构恰好就是 LRU 的一种实现。

#### TreeMap

* TreeMap 内部按照 key 的自然升序进行排序，如果需要按照倒序排序，需要在构造 TreeMap 时传入 Comparator 比较器来作为 TreeMap 的排序依据。Comparator 接口的 compare() 是具体的比较方法
* compare() 一般会调用待比较对象的 compareTo() 来进行比较，而 compareTo() 是 Comparable 接口的方法
* 所以 TreeMap 中的 key 都需要实现 Comparable 接口，当然例如 String / Integer 等都实现了 Comparable 接口，提供了默认的 compareTo()，这些对象作为 key 时可以直接使用
* TreeMap 的底层为红黑树，而红黑树的平衡操作不会改变它的中序遍历顺序，所以使用中序遍历的结果就是按照比较器排序后的结果

## fail-fast / fail-safe

fail-fast 是一种立即报告故障的机制，如在一个方法中做除法运算，只要检测到除数为 0 就立刻抛出运行时异常。这种机制可以事先识别出一些错误。

而 Java 中的 fail-fast 机制一般指集合类中的错误检测机制，当多线程修改这种容器时就会抛出 ConcurrentModificationException，但是需要注意的是，单线程下也会触发 CME：

* 在 foreach 中对容器元素进行 add / remove 操作

这是因为 foreach 实际上使用了 iterator 来遍历集合，而 add / remove 则是直接调用了容器自身的方法，导致 iterator 在遍历时发现存在自己未知的元素增删操作，所以抛出异常。

这种情况通常是需要对元素进行过滤，避免这种问题可以考虑如下方法：

* 使用 iterator 提供的 remove()
* 将集合转变为流，通过流过滤的元素会形成新的流
* 使用 fail-safe 容器

fail-safe 的容器在需要遍历时会对容器进行拷贝，而遍历操作就是在这份拷贝上进行。

## COW

CopyOnWriteArrayList / CopyOnWriteSet 都是基于 COW 的 fail-safe 容器：

* 多线程访问共享资源，当某个线程需要修改内容时，会将内容拷贝出一个副本然后去修改
* 其他线程读取旧的内容，直到写线程将内容修改完毕

Copy-On-Write 容器的基本原理：

* volatile 修饰容器，保证可见性，使得修改能够立即被其他线程可见
* 修改时加锁，然后令原容器的引用指向新的容器副本

需要注意的是，这类容器的 add / remove 都是加锁的，因为拷贝是写操作触发的，所以写操作时加锁是为了防止拷贝出多个副本导致并发写。

这种容器存在一些弊端：

* 写操作时内存中存在两个容器，如果占用空间较大，可能会造成 GC 从而影响性能
* 读操作不加锁，所以只能保证最终一致性而不能保证强一致性

另外，Redis 进行 RDB 方式的持久化时也利用了这个机制：子进程进行持久化操作时，如果主进程接收到写操作时，会 copy 出一份内存进行写操作。
