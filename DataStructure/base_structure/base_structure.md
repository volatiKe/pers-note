## 数组

**数组**：一种线性表结构，用一组连续的内存空间，存储相同类型的数据

**数据的特点**：由于其内存分配连续，并且存储的数据类型相同，所以具有随机访问的特点，能够通过下标在 O(1) 内进行随机访问。需要注意的是，对元素的查找并不是 O(1)

```java
a[i]_address = base_address + i * data_type_size
```

**对数组的操作**：

* 在数组内数据无序的情况下，在进行数据插入操作时，为了避免大规模数据移动，可以将第k位的数据移动至元素列末尾，将插入的元素放入第 k 位
* 同样为了避免删除第 k 位元素导致内存不连续而需要移动数据的情况发生，在某些不追求数据连续性的情况下，可以多次标记待删除的数据，然后只触发一次删除操作

**数组与容器**：容器的好处在于将数组操作的细节进行封装，如 ArryaList 能够在空间不够时进行 1.5 倍的扩容，但扩容会进行数据的复制，影响性能，所以使用时最好指定容器大小

## 链表

### 链表访问计数

```java
ListNode node4 = new ListNode(4, null);
ListNode node3 = new ListNode(3, node4);
ListNode node2 = new ListNode(2, node3);
ListNode node1 = new ListNode(1, node2);
ListNode vir = new ListNode(-1, node1);
```

#### 有虚拟头节点

##### 直接计数

```java
int n = 3;
ListNode cur = vir;
while (n > 0) {
    cur = cur.next;
    n--;
}
// 3
System.out.println(cur.val);
```

* n 减 3 次后跳出循环，即 cur 从 vir 开始走 3 步，指向 3

##### 统计数量

```java
int count = 0;
ListNode cur = vir;
while (cur.next!=null) {
    cur = cur.next;
    count++;
}
// 4
System.out.println(count);
```

* cur 指向 4 时，count 为 4，此时需要跳出循环，所以循环终止条件为 cur.next!=null

##### 快慢指针

```java
ListNode slow = vir;
ListNode fast = vir;
while(fast!=null && fast.next!=null){
    slow = slow.next;
    fast = fast.next.next;
}
```

* 奇数个节点：slow 位于中心
* 偶数个节点：slow 偏左 

## 队列

**循环队列**：数组实现

* 判空：`head == tail`
* 判满：`head == (tail + 1) % n`
* 更新指针：`p = (p + 1) % n`

**链式队列**：由于链表的特点，可以实现一个支持无限排队的无界队列，但是可能会导致请求排队过多，处理过慢，如 Java 线程池中的 LinkedBlockingQueue

**顺序队列**：基于数组实现，队列大小有限，队满则拒绝，适合对响应时间敏感的系统，但队列的大小需要合理设置以充分利用资源，如 Java 线程池中的 ArrayBlockingQueue

## 堆

堆是以数组形式存储的一棵完全二叉树，每个节点的值大于等于（或小于等于）其左右节点的值。

上浮：

```java
// 只要父节点还比子节点小就不断交换，交换后父节点一定不比两个子节点小
// 因为一个是曾经的父节点，一个是比曾经父节点小的子节点
while(k > 1 && a[k/2] < a[k]){
    swap(a, k, k/2);
    k = k/2;
}
```

下沉：

```java
// “=”保证左子节点存在
while(2 * k <= n){
    int j = 2 * k;
    // “<”保证右子节点存在
    // 右子节点更大的话需要更新指针
    if(j < n && a[j] < a[j+1]){
        j++;
    }
    // 如果父节点不再小于子节点就停止循环
    if(a[k] >= a[j]){
        break;
    }
    swap(a,j,k);
    k = j;
}
```

**插入元素**：将新元素添加到数组末尾，更新堆的大小，并将新元素上浮至合适的位置

**删除堆顶元素**：删除顶部元素后，将末尾元素置于堆顶，更新堆的大小，并让新的堆顶元素下沉至合适位置

以上两个操作的主要过程就是恢复堆的有序性，与完全二叉树的高度成正比，所以时间复杂度均为 $O(log_{2}n)$
