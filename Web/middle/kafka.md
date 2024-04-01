## 场景

* 异步：使用离线任务进行卡券发放，借助消息队列异步进行发放和 push 的逻辑
* 解耦：消费交易消息进行持仓的加减
* 限流：用户参加活动的请求进入消息队列，业务顺序处理

## 架构

* topic：逻辑上的队列，维护一个主题的消息
* parition：物理上的队列，为了提高吞吐量，一个 topic 有多个 partition，每个 partition 内容不同

## 可靠性

broker
* 对消息的持久化存储
* 每个 partition 在多个节点上都有副本

consumer
* 手动提交 offset，kafka 记录该 offset