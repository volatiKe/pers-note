# IntelliJ 配置

---

## 全局配置

* 修改 VM 参数
  > help -> edit custom VM options

  ```null
  针对 16G 机器
  -Xms512m
  -XX:ReservedCodeCacheSize=500m
  ```

* 更换全局默认 jdk 版本
* 设置自动切换 properties 文件为 ascii 编码
  > file encodings
* 自动导包
  > auto import -> add unambiguous import on the fly / optimize imports on the fly
* 设置代码模板
    > editor -> file and templates -> includes -> file header

    ```java
    /**
    * @author panke
    * @date ${YEAR}/${MONTH}/${DAY}
    */
    ```

* 代码修改后目录出现颜色变化
  > version control -> show directions with changed descendants
* 禁止 import *
  > editor -> code style -> java -> import -> class count to use import with \* / names count to use static import with * -> 999
* 注释起始增加空格
  > editor -> code style -> java -> code generation -> add a space at comment start
* 换行显示方法链式调用
  > editor -> code style -> java -> wrapping and braces -> chained method calls -> chop down if long

## 项目配置

* 自动提示忽略大小写
  > editor -> general -> code completion -> match case
* 允许 tabs 多行显示
  > editor -> general -> editor tabs -> show tabs in one row
* 新打开的 tab 显示在末尾
  > editor -> general -> editor tabs -> open new tabs at the end
* 允许使用滚轮修改字体大小
  > editor -> general -> change font size with ctrl+mouse wheel
* 设置代码语句快捷键
  > editor -> general -> postfix completion
* 不自动打开上一次打开的项目
  > appearance & behavior -> system settings -> reopen last project on startup
* 关闭代码拖拽
  > editor -> general -> enable drag'n'drop functionality in editor
* 显示内存使用情况
  > appearance & behavior -> appearance -> show memory indicator
* 增加 tab 数量
  > editor -> general -> editor tabs
* 单行代码不折叠
  > editor -> general -> code folding -> one-line methods
* Pin Tab 快捷键
  > ctrl + option + p

## 插件

* Ali 代码规范
* CodeGlance：代码滚动条
* Rainbow Brackets：括号匹配上色
* .ignore：生成.gitignore文件
* Free Mybatis：自动关联 mapper 文件
