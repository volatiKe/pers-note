# xml

## 如何理解这些标签

```xml
<project xmlns="http://maven.apache.org/POM/4.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://maven.apache.org/POM/4.0.0 http://maven.apache.org/xsd/maven-4.0.0.xsd">
```

首先需要明确的是 xml 文件中的每个元素都必须有所属的 namespace，否则无法区分同名元素

* xmlns
  * 表示无前缀元素 namespace，值是 namespace 的唯一标识
* xmlns:xsi
  * 表示 xsi 前缀元素 namespace
  * xsi 是一个业界默认的 namespace，用来定义一些元数据，如 xsi:schemaLocation
* xsi:schemaLocation
  * 引用 xsd 文件来校验文档的格式，空格前是 namespace 的唯一标识，空格后是对应 xsd 文件的物理位置
