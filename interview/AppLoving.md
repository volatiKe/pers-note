一面

清结算架构
清算规则变化的时候怎么处理

leetcode 39（40）

滑动窗口限流

* redis zset
	1. 每个 uid 一个 zset
	2. 单个 uid 每次每次访问落一个 kv，v 为时间戳
	3. 如果当前时间 n 分钟前到当前时间的 kv 数量超过阈值，则拒绝
	4. 否则在 zset 中加入 kv
* 环形队列
	* 使用一个循环队列存放请求，队列的大小为 1s 内允许的请求次数
	* 每当请求到来时删除与这个新请求间隔 1s 的旧请求（头节点后移）
	* 如果队列中无空位存放新来的请求，则阻塞

二面

leetcode 210

重构建模的依据或准则
* 确定领域
* 抽象业务（如经办复核可以看成资金推进）
* 功能平迁
业务模型？
如何实现 redis 加锁，自己封装的问题
	* redis 锁不可重入
	* 不能续期
	* 阻塞的
为什么 redis 快 
	io 多路复用
	基于内存
	高效的数据结构