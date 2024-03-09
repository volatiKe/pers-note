表达
* 慢语速
* 分点描述

系统介绍
* 专门面向度小满理财的基金交易中的付款的异步清结算流程，没有直接的用户流量
* 根据资金位置的不同分为：
	* 上行：支付 -> 销售
	* 下行：销售 -> 支付

系统架构
* 在线服务 + 定时任务
* 在线服务：
	* 下行清算、结算
	* 销售清结算
* 定时任务：
	* 上行清算、结算
	* 对账

业务流程：
* 支付：
	* 卡支付 + 日切后百付宝渠道上行 + T+1 其他渠道上行 + T+1 的正向数据报送
* 清结算：下行款
	* 交易确认后清算
		* 解析新交易核心数据进行逆向数据报送
		* 旧交易核心在处理完确认后触发下行清算

业务难点：
* 场景衔接：
	* 资金回来：
		* 基金买余额盈
		* 基金买余额盈提前转购
		* 基金买余额盈+T05的提前转购
* 资金不回来：
	* 赎转购：
		* 识别赎转购数据进行二清（根据公式计算转购时间）
		* paycenter + trade 走转购和申购流程
		* 转购失败需要走 paycenter 退款（资金未到）
		* 清算消费消息得知转购结果
		* 完善监管文件数据并判定数据完整性
		* 转购失败的数据走下行款流程付款转退款

技术难点：
* 销售结算批处理的可重入性：
	* 数据可修改，所以有重复出批次的 case，需要取消之前的批次
	* 重出批次由人工触发，不可能阻塞页面
	* 所以取消的实际逻辑是将批次作废
	* 而明细每次都使用 insertOrUpdate 的方式处理
	* 同时明细字段携带版本号，有效明细的版本号必须为空、当前批次（重入）、当日无效

为什么重构（项目价值）
* 收敛服务、配置、解耦 activiti 引擎，降低维护成本
* 重新确定领域边界
* 便于后期承载更多清结算业务逻辑

重构做了哪些事
* 功能的平迁
* 确认领域职责，迁出领域外逻辑，抽象清结算流程的模型
* 切流

遇到的问题、bug
* 清结算：
	* 切流时，资金流数据要和信息流一致
		* 只对信息流打标
			* 资金先到：信息流不打标
			* 资金流没有到：信息流打标
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

优缺点

优点
1. 架构清晰，一清二清解耦，每个流程都有解析、校验、清算（三要素）
2. 领域划分明确

改进
1. 大文件处理
2. 信息流不必过结算
3. 只支持文件方式清结算（接口 或 消费消息 的明细处理）

未来
* 实时付款的清结算
* 结算单引入结算时间，结算 + 账务

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
		* 分布式锁内有事务，但走了内部调用未生效
		* 用了spring event 中的 after commit，spring 自动开了新事务
		* 以上两点导致出现了事务在锁外边的情况
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
  and serial_no > ''
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
猜测一：
是找最小值是回表后维护小顶堆即可
但是 order by limit 一定会走 filesort（内存或磁盘排序），此 case 下堆排序比 filesort 快

猜测二：
走了索引合并，因为 serial 联合索引查出来的都是有序的 id，可以直接求 id 交集，但是 serial>'' 拿到的 id 过多，所以比较耗时，而后续 serial_no 有值的 sql 就执行的快多了



系统核心指标：
每个系统都有它最核心的指标。比如在收单领域：进件系统第一重要的是保证入件准确，第二重要的是保证上单效率。清结算系统第一重要的是保证准确打款，第二重要的是保证及时打款。我们负责的系统是美团点评智能支付的核心链路，承担着智能支付 100%的流量，内部习惯称为核心交易。因为涉及美团点评所有线下交易商家、用户之间的资金流转，对于核心交易来说：第一重要的是稳定性，第二重要的还是稳定性。

准确：对账
实效：监控
（稳定性）

系统稳定性保证：
* 监控：服务端口，数据状态，接口错误率
* 安全：diam
* 可用：集群部署、bns 负载均衡

对账：事后对账
* 主要对账：账户余额：综合当日15点时各类基金的清结算、支付、交易数据计算的未付款和15点余额对账
* 其他对账：系统间两两对账
	* 账证：清结算、支付 vs 交易单
	* 账账：商户出款、入款，商户余额
	* 账实：支付流水和商户流水

想做（准实时）需要账务系统加入，做专款专用

如何保证资金实效
* 定时任务 + 人工审批 + 系统自动批处理
* 主要靠监控发现问题

资损防控会做哪些手段设计保证？
* 人工操作：
	* double check 等
* api 加固：
	* 接口调用：diam
	* 文件的 md5
* 系统设计：
	* 错误码判定：只判定明确成功和明确失败，非预期错误码直接人工介入
	* 幂等控制：明确幂等错误码，幂等id的唯一性控制
	* 防并发：一锁二判三更新，数据库底层唯一索引兜底
	* 一致性：事务，状态机控制下的查单补偿、直接补偿，主从延迟，对账
	* 兼容：新老服务切换，新老数据的兼容性
	* 状态机：终态不可逆，状态推进前的准入判断
	* 业务处理：有收才有退、业务校验
	* 流程设计：人工保证的专款专用
* 运维
	* 线上线下环境隔离