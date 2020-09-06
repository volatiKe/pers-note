# MySQL 基础

---

## 1. SQL

聚合函数：

* 为什么 where 子句中不能出现聚合函数？
  聚合函数是列函数，基于整列进行计算的，前提是结果集已经确定，而 where 是对行数据进行过滤，是正在确定结果集的过程，所以不能使用聚合函数
* SQL 中各子句的执行顺序：**from -> where -> group by -> having -> order by -> select**

去重（DISTINCT）：

```sql
select distinct <col> from <table>
```

col 中重复的内容只出现一次，重复指的是查询的所有 col 的值。
都相同。

```sql
count(distinct <col>)
```

统计某个字段去重后包含多少个值。

分组（GROUP BY）：

* 针对同一字段，具有相同值的归为一组
* **除了汇总用到的字段外，SELECT 的每个字段都必须在 GROUP BY 子句中给出**

过滤：

* 行过滤（WHERE）：按条件対行进行过滤
* 组过滤（HAVING）：対分组后的结果进行过滤
* **行过滤先于组过滤**

排序（ORDER BY）：

* 対分组的结果按照指定的字段进行排序
* 过滤操作先于排序，也就是：where -> group by -> having -> order by

连接：

![join](img/join.jpg)

```sql
select <col>
from <table_a> (inner/left/right/full) join <table_b>
on <table_a.col> = <table_b.col>
```

* 返回结果中，如果另一张表没有匹配的记录，则用 null 填充
* 内连接就是等值连接，可以使用 WHERE 代替

    ```sql
    SELECT <table_a.col>, <table_b.col>
    FROM <table_a>, <table_b>
    WHERE <table_a.col> = <table_b.col>
    ```

子查询：

* 值子查询：**子查询结果为一行一列**，凡是可以指定值的地方都可以替换为值子查询

  ```sql
  SELECT *
  FROM <table>
  WHERE <col> = (SELECT <col> FROM <table> WHERE <col> = <const>)
  ```

* 行子查询：**子查询结果为一行多列**

  ```sql
  SELECT *
  FROM <table>
  WHERE (<col1> <col2>) = (SELECT <col1>,<col2> FROM <table> WHERE <col> = <const>)
  ```

* 列子查询：**子查询结果为多行一列**，由于查询结果是多行，不能使用运算符

  ```sql
  SELECT *
  FROM <table>
  WHERE <col> IN (SELECT <col> FROM <table> WHERE <col> = <const>)
  ```

* 表子查询：**子查询结果为多行多列**，子查询替换 FROM 后的表名
* 子查询不包含 ORDER BY 子句，ORDER BY 只能作用于主 SELECT

组合（UNION）：

* 如果第一个查询返回 M 行，第二个查询返回 N 行，那么组合查询的结果一般为 M + N 行
* 每个查询必须包含相同的列、表达式和函数
* 默认会去除相同行，如果需要保留相同行，使用 UNION ALL
* 只能包含一个 ORDER BY 子句，并且必须位于语句的最后

## 2. 范式

1NF：属性不可分，也就是说不能出现单一字段下出现二级字段。

2NF：任何一个非主属性都必须和所有主属性相关，主属性（们）共同标识一行记录。

3NF：非主属性只能依赖主属性，不能依赖其他非主属性。

## 3. MySQL 存储引擎

|          | InnoDB             | MyISAM                   |
| -------- | ------------------ | ------------------------ |
| 索引     | 聚簇索引           | 非聚簇索引，不支持外键 |
| 事务     | 支持               | 不支持                   |
| 锁       | 支持行锁           | 仅支持表锁               |
| 使用场景 | 可靠性高、读写频繁 | 无事务、频繁查询         |
