# Segmentation

## 问题引入

[[mechanism_address translation]] 中提到的地址转换存在一个问题：

* 将整个地址空间映射进物理内存中后，栈和堆之间的没有被使用的空闲内存也占用了实际物理内存

可见，base & bounds register 的机制不够灵活

## 分段

为了解决上述空闲内存占用物理内存的情况，引入了分段机制：

* 每个地址空间分为三个段：code、stack、heap
* 每个段使用一对 MMU

| 地址空间                                               | 物理内存                                               | 
| ------------------------------------------------------ | ------------------------------------------------------ |
| ![[img/segmentation/Snipaste_2022-06-14_19-22-47.png]] | ![[img/segmentation/Snipaste_2022-06-14_19-24-35.png]] |

| segment | base | bounds |
| ------- | ---- | ------ |
| code    | 32KB | 2KB    |
| heap    | 34KB | 3KB    |
| stack   | 28KB | 2KB    |

### code segment 访问

假设访问 code segment 100：

1. 地址空间 code segment 从 0 开始，访问 100，那么 offset  为 100
2. 虚拟地址 100 对应物理地址为 32KB + offset = 32868

### heap segment 访问

假设访问 heap segment 4200:

1. 地址空间 heap segment 从 4KB 开始，访问 4200，那么 offset 为 4200 - 4KB = 104
2. 虚拟地址 4200 对应的物理地址为 34KB + 104 = 34920

## 如何选取段

硬件在进行地址转换时，需要知道段内地址的 offset，以及该访问哪个段：

![[img/segmentation/Snipaste_2022-06-14_19-44-16.png]]

| segment   | base | bounds |
| --------- | ---- | ------ |
| code(00)  | 32KB | 2KB    |
| heap(01)  | 34KB | 3KB    |
| stack(11) | 28KB | 2KB    |

再来看 heap segment 访问 4200 的例子，4200 的二进制为：01 0000 0110 1000

* 其中 01 表示此段为 heap segment
* 0000 0110 1000 为 104，即之前计算的 offset，**可以认为 4KB 处是 0，那么向内存增长方向 104 处为该 offset 位置**

硬件只需要根据前虚拟地址前 2 bit 获取段，后 12 bit 自然就是 offset，加上 base register 的值即可得到物理地址：

* bounds register 的 check 也发生了变化：只需用此 offset 值和 bounds register 的值去比较，不大于即非越界
* 2 bit 用来区分段，导致该段实际可用的地址空间大小从 16KB 变为 4KB

## stack segment 访问

stack 的地址空间和 heap、code 相比为反向增长，所以需要扩充硬件支持：

| segment | base | bounds | grows positive |
| ------- | ---- | ------ | -------------- |
| code    | 32KB | 2KB    | 1              |
| heap    | 34KB | 3KB    | 1              |
| stack   | 28KB | 2KB    | 0              |

假设访问 stack segment 15KB：

1. 地址空间 stack segment 从 16KB 开始，访问 15KB，那么 offset 为 15KB - 16KB = -1KB
2. 虚拟地址 15KB 对应的物理地址为 28KB + (-1KB) = 27KB

得知物理地址是 27KB 后，再来分析硬件计算过程，15KB 的二进制为：11 1100 0000 0000

* 其中 11 表示此段为 stack segment
* 1100 0000 0000 为 3KB，显然这个值不是正确的 offset
*  这个 3KB 其实是在 stack segment 内相对于 4KB 大小的 offset，**可认为 16KB 处是 0，那么向内存增长方向 3KB 处为该 offset 位置**。由于 stack segment 是反向增长，所以 offset 为 3KB - 4KB = -1KB
* 得到正确的 offset 后，加上 base register 的 28KB，即得到了 27KB 的物理地址

同样地，和 bounds register 比较的是 offset 的绝对值

## 段共享

存放只读数据的段（如 code segment）可以被多个进程共享，需要扩充硬件支持：

| segment | base | bounds | grows positive | protection   |
| ------- | ---- | ------ | -------------- | ------------ |
| code    | 32KB | 2KB    | 1              | read-execute |
| heap    | 34KB | 3KB    | 1              | read-write   |
| stack   | 28KB | 2KB    | 0              | read-write   |