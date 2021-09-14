# Limited Direct Execution

为了虚拟化 CPU，OS 需要在许多看起来同时运行的进程间共享物理 CPU 资源，这就引入了两个问题：

* 性能：如何在不给系统增加过多开销的情况下实现虚拟化
* 控制：如何在保持对 CPU 控制的同时有效地运行进程

因此，在保持对 CPU 控制的同时获得高性能是构建 OS 的关键点之一

## Direct Execution

Direct Execution 指**直接在 CPU 上运行程序**，这样能够使得程序运行的尽可能块

|OS|program|
|---|---|
|在进程列表中创建入口||
|为程序分配内存||
|将程序加载至内存||
|设置栈||
|执行对程序入口 (main()) 的调用||
||运行 main()|
||执行 return|
|释放进程内存||
|从进程列表中移除||

## Direct Execution：受限操作的执行

如果程序直接运行在 CPU 上，就可以不受限制地操作硬件与 OS，如：访问磁盘进行 I/O 请求，这显然是不可行的。由此引入「用户态」&「内核态」的概念：

* 首先需要明确的是：**这需要 CPU 的硬件支持**
* 在用户态下程序无法执行受限的 CPU 指令，只能通过执行「系统调用」来执行
* 为了执行系统调用，需要先执行 trap 指令来使 OS 从用户态「陷入」内核态；进入内核态后 OS 可以执行受限指令，完成进程的任务后 OS 执行 return-from-trap 返回用户态
* 内核需要在引导时建立 trap table 以保证用户程序执行 trap 指令后能跳转到对应地址进行 trap 操作
* kernel stack：内核栈，进程触发系统调用后，内核态下执行代码所使用的栈

|OS@boot(kernel mode)|Handware|Program(user mode)|
|---|---|---|
|初始化 trap table|||
||记住系统调用处理器的位置|||

|OS@run(kernel mode)|Handware|Program(user mode)|
|---|---|---|
|在进程列表中创建入口|||
|为程序分配内存|||
|设置 user 栈|||
|将寄存器信息保存至 kernel 栈|||
|return-from-trap|||
||将寄存器从 kernel 栈中恢复||
||切换为 user mode||
||跳转至 main()||
|||运行 main()|
|||执行系统调用|
|||trap|
||将寄存器信息保存至 kernel 栈||
||切换为 kernel mode||
||跳转至 trap 处理器||
|处理 trap|||
|执行系统调用|||
|return-from-trap|||
||将寄存器从 kernel 栈中恢复||
||切换为 user mode||
||跳转至 trap 前的位置||
|||return from main()|
|||trap(via exit())|
|释放内存|||
|从进程列表中移除|||

## Direct execution：进程切换

**微小的问题**：当 CPU 在运行用户程序时，意味着 OS 没有被运行，那么 CPU 如何进行进程切换的操作呢？

### 非抢占式：等待发生系统调用

程序显式地进行 yield 系统调用，让出 CPU 资源，但这种方式存在问题：如当程序出现了死循环。

### 抢占式：OS 进行切换

* OS boot 时设置一个定时器，可以定时发出 interrupt
* 出现 interrupt 时硬件去 trap table 中找 interrupt handler

|OS@boot(kernel mode)|Handware|Program(user mode)|
|---|---|---|
|初始化 trap table|||
||记住系统调用处理器、时钟处理器的位置||
|开始时钟中断|||

|OS@run(kernel mode)|Handware|Program(user mode)|
|---|---|---|
|||A 进程运行|
||时钟中断||
||将寄存器中 A 进程的数据保存至 A 进程的 kernel stack||
||切换至内核态||
||跳转至 trap 处理器||
|处理 trap|||
|进行进程上下文切换|||
|（1）将寄存器中 A 进程数据保存至 A 的进程结构体中|||
|（2）从 B 的进程结构体中将数据恢复至寄存器中|||
|（3）使用 B 进程的 kernel stack|||
|return-from-trap|||
||将 B 进程 kernel stack 中的数据恢复至寄存器中（因为此时 OS 指向的 kernel stack 为 B 进程的）||
||切换至用户态||
||跳转至 B 进程的 PC 指向处（因为此时寄存器中保存的是 B 进程的 PC）||
|||B 进程运行|

上述过程为中断处理中进行了进程上下文切换：

* 保存现场：出现时钟中断时硬件将当前正在运行进程的寄存器数据保存至进程的 kernel stack中
* 中断处理：进程切换，也可以是其他逻辑
* 恢复现场：从kernel stack中恢复之前保存的现场，由于中断处理中进行了进程切换，所以恢复的是其他进程的现场
