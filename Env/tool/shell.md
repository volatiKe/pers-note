# Shell 常用命令

## 脚本执行

需要知道的：

1. export 用于新增当前 shell 的环境变量
2. 被 export 出来的变量虽然可以被子 shell 使用，但只是一个拷贝，不会影响到父 shell 中的值以及其它子 shell 中的值
3. 一个 shell 中的系统环境变量才会被复制到子 shell 中（用 export 定义的变量）
4. 一个 shell 中的系统环境变量只对该 shell 或者它的子 shell 有效，该 shell 结束时变量消失（并不能返回到父 shell 中）
5. 不用 export 定义的变量只对该 shell 有效，对子 shell 是无效的

|执行方式|是否开启子 shell|脚本中的变量可读性|是否需要脚本的可执行权限|
|---|---|---|---|
|source（或```.```)|否|只是简单地读取脚本的语句并依次在当前 shell 中执行，没有建立新的子 shell。所以脚本里面所有新建、改变变量的语句都会保存在当前 shell 中|不需要|
|bash（或 sh、zsh)|是|该子 shell 继承父 shell 的环境变量，但子 shell 新建的、改变的变量不会被带回父 shell|不需要|
|./|是|同上|需要|

## sed

sed 的处理单位为**一行**，命令使用```''```包裹

### 参数

* **-n**:sed 默认在执行完命令后将输入的每一行打印在标准输出上，此参数避免了此行为
* **-i**: 所有的改动在源文件上执行，输出覆盖源文件

### 内容

* **s/内容 1/内容 2/**: 将内容 1 替换为内容 2
* **g**: 默认 sed 只处理每行匹配到的第一个内容，g 表示 global，处理全文内容，如```s/xxx/xxx/g```
* **p**: 打印行，一般和```-n```一起使用，如```sed -n '1,3p' input-file```为打印文件 1 到 3 行

## awk

awk 的处理单位为**一行**，根据分割符分解成多个域，再对每一个域根据命令进行处理

命令范式

```shell
awk -F '分隔符' '{命令}' input-file
```

其中：

* -F 可省略，表示分隔符为空格
* ```$0```表示整行内容
* 分割后的列从```$1```开始
* 内置变量：
  * NF: 一行分割后域的个数
  * NR: 行号

## curl

* **-X**: 指定请求方式，不写时默认 GET，如```curl -X POST https://www.baidu.com```
* **-H**: 指定请求头，如```-H 'Content-Type: application/json'```
* **-d**:POST 请求时指定请求体，需要注意的是这个参数会自动加上```Content-Type : application/x-www-form-urlencoded```这个请求头，并且转换为 POST 请求
* **-o**: 将请求的响应保存为文件，如```curl -o filename.html https://www.baidu.com```，相当于```wget```
