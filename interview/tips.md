表达
* 慢语速

系统介绍
* 只面向度小满理财，专注于解决基金产品在交易过程中的资金流管理
* 处于交易和理财支付之间
* 根据资金位置的不同分为：
* 上行：支付 -> 销售
* 下行：销售 -> 支付
* 面向付款的异步清结算流程，没有直接的用户流量

业务流程：
* 支付：卡支付
* 清结算：下行款
	* 交易确认后清算
		* 一清结果报监管
		* （离线任务出信息流和资金流文件）也可以二清结果做信息流，一清结果走资金流

为什么重构（项目价值）
* 收敛服务、配置、解耦 activiti 引擎，降低维护成本
* 重新确定领域边界
* 便于后期承载更多清结算业务逻辑

重构做了哪些事
* 功能的平迁
* 确认领域职责，迁出领域外逻辑，抽象清结算流程的模型
* 切流

遇到的问题、bug
* 支付中台：
* 清结算：
	* 切流时，资金流数据要和信息流一致
		* 只对信息流打标
			* 资金先到但没打标：信息流不切
			* 资金流没有到则对信息流打标
		* 资金流处理
			* 查信息流是否打标
			* 查不到信息流（资金流先到）则不打标

资金流先到的场景：
* 5002 带回来了用户赎回
* 用户赎回到余额盈
* 上午触发了结算但转购的信息流还没跑


功能
* 清算：三要素
* 结算：资金位置的感知、推进、触发划拨
* 支付：抽象度小满支付的资金划拨能力、资产互转

架构
1. 交易侧文件给到清算
2. 清算解析文件、校验、清算、通知结算（批处理）
3. 清算直接或异步通知结算开启结算
4. 结算被动接受请求或主动调用来感知或推进资金位置（批次维度）
5. 结算触发资金划拨（明细维度）
6. 支付回调结算，结算回调交易

优缺点

优点
1. 架构清晰，一清二清解耦，每个流程都有解析、校验、清算（三要素）
2. 领域划分明确

改进
1. 大文件处理
2. 信息流不必过结算
3. 只支持文件方式清结算（接口方式的明细处理）

亮点
1. 引入一清、二清的概念，可以（赎转购）将销售清结算和支付清结算打通
2. 无资损、不影响业务地分阶段切流

切流
* 直接从切 settle 说
* 流量复制
	* 清算修改代码
	* 清算双写老结算
	* 清算双写老新
	* 清算单写老和新
	* 清算单写新
* 其他
	* 老结算切一部分到新支付
	* 单写老和新时加锁互查
	* 对外汇总金额
	* 老转发请求到新
	* 写新老表由配置驱动 dao 层切换

成长
* 独立负责单一领域核心服务的重构全流程，梳理掌握理财清结算全业务
* 重构里程碑多，需要协调、考虑各方资源
* 对资金服务的各种变更需要有完整、规范的流程来避免问题发生

技术点
* 反射解析文件
	* 具体的文件解析器持有文件行 model 的泛型类
	* 泛型类的构造方法入参是文件行 String
	* 父类逐行读取文件内容后直接通过 class.newInstance 构建文件 model 实体类
	* 整个循环读一行处理一行
	* 统计行数和金额 & 读取过程中如果有与配置相同的结束符则放入 TL
	* 行数、金额、结束符 check 不过则抛异常
* 线程池监控
	* 线程池放入 map
	* Spring 定时任务根据 cron 的配置从 map 中拿出线程池打印参数到单独的日志
	* 运维平台配置规则解析日志，统计参数
*  线程池动态调参
	* 线程池放入 map
	* apollo 配置中心支持服务监听配置修改事件
	* 监听到线程池参数配置修改后从 map 中获取对应线程池并设置新的参数
* Spring 消息机制解耦一清二清
	* 一清结果请求下游前触发二清
	* 根据数据业务类型从业务工厂拿到 event
	* publish event
	* @EventListener 监听事件
	* 监听事件构建数据落库前方法加锁做幂等判断防止一清结果请求下游失败
* 迭代聚合
	* 文件需要根据 db 中某日所有数据按多维度聚合出 n 条数据
	* 每页捞 500 条聚合一条中间数据
	* 下一页捞出 500 条再聚合成一条中间数据
	* 中间数据再次聚合，最大程度减少内存中数据占用
* 树形结构多维度拦截器
	* 配置为 json 格式，operator & left & right，可以多层嵌套
		* {operator: and, left:{userId:xxx},right:{operator:or,left:xxx,right:{orderNo:xxx}}}
	* 有生效开关配置，必须在开关 ON 时才启用，开关实时读取配置
	* 配置的 key 拼入批次号，第一次构建拦截树后以批次号存入本地缓存
		* curl 接口刷新
		* 默认 24h 
	* 构建
		* 有 operator key，则递归调用构建方法获取 left 和 right key 构建左右节点，节点类型为 OperatorNode 并返回
		* 否则构建 ConditionNode，存放各种维度数据的 Set，配置中的白名单数据直接读进 Set 中
		* 判断时调用两个节点的公共方法 getResult
			* ConditonNode 的此方法是根据数据字段是否在对应 Set 中
			* OperatorNode 的此方法是 left.getResult && right.getResult 或者 left.getResult || right.getResult
* redis 线程可重入锁
	* 加锁
		* 从 TL 中取出 map，根据 key 获取次数
		* 如果次数是 0
			* setNx 放入 key & value & 过期时间
			* 加锁成功次数 +1，返回 true
			* 加锁失败则 map 中删掉此 kv，并返回 false，业务抛异常
		* 如果次数不是 0
			* 次数 + 1，返回 true
	* 解锁
		* 从 TL 中取出 map，根据 key 获取次数
		* 次数为 1
			* 删掉 redis 的 key
			* redis 操作成功则删掉 map 的 kv，返回 true
			* 否则返回 false，，业务抛异常
		* 次数大于 1
			* 次数 -1，返回 true

一些问题
* 余额 500 分
	* binlog
	* 事务
	* [3、资产互转表500分未清零问题排查（RR隔离级别下的脏读） (duxiaoman-int.com)](https://zhishi.duxiaoman-int.com/knowledge/HFVrC7hq1Q/eyzDp1Jx9p/7Gz5kQ4LYp/shCpvgbC8sjRtG)
* 引入 fifa 导致流程引擎不开启
	* springboot 启动
	* [20220601-引入 fifa 导致流程不开启 (duxiaoman-int.com)](https://zhishi.duxiaoman-int.com/knowledge/HFVrC7hq1Q/eyzDp1Jx9p/Zc41lh8kjF/ZFw3eReyp9mlm1)
* 慢查
```
explain  
select *  
from pay_settle.sales_settle_detail  
where report_date = '20240116'  
  and direction = 1  
  and clear_ta_code = '01'  
  and order_no > ''
order by serial_no  
limit 10;
```

| | |
| :- | :- |
| **id** | 1 |
| **select\_type** | SIMPLE |
| **table** | sales\_settle\_detail |
| **partitions** | null |
| **type** | ref |
| **possible\_keys** | idx\_report\_date\_direction\_ta,idx\_report\_date\_direction\_fund |
| **key** | idx\_report\_date\_direction\_ta |
| **key\_len** | 133 |
| **ref** | const,const,const |
| **rows** | 1 |
| **filtered** | 100 |
| **Extra** | Using index condition; Using where; Using filesort |
```
explain  
select min(serial_no)  
from pay_settle.sales_settle_detail  
where report_date = '20240116'  
  and direction = 1  
  and clear_ta_code = '01';
```
| | |
| :- | :- |
| **id** | 1 |
| **select\_type** | SIMPLE |
| **table** | sales\_settle\_detail |
| **partitions** | null |
| **type** | ref |
| **possible\_keys** | idx\_report\_date\_direction\_ta,idx\_report\_date\_direction\_fund |
| **key** | idx\_report\_date\_direction\_ta |
| **key\_len** | 133 |
| **ref** | const,const,const |
| **rows** | 1 |
| **filtered** | 100 |
| **Extra** | null |
猜测是找最小值是回表后维护小顶堆即可
但是 order by limit 一定会走 filesort（内存或磁盘排序）
加了 order_no 有索引，其实差不多，但是最坏情况（数据量很大时）肯定堆排序更快

每个系统都有它最核心的指标。比如在收单领域：进件系统第一重要的是保证入件准确，第二重要的是保证上单效率。清结算系统第一重要的是保证准确打款，第二重要的是保证及时打款。我们负责的系统是美团点评智能支付的核心链路，承担着智能支付 100%的流量，内部习惯称为核心交易。因为涉及美团点评所有线下交易商家、用户之间的资金流转，对于核心交易来说：第一重要的是稳定性，第二重要的还是稳定性。