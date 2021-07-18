# maven

## 如何理解 lifecycle 和 plugin 的关系

### lifecycle

* maven 有三套生命周期：clean、default、site，每个生命周期都有若干个阶段（phase）构成
* 三个生命周期互相独立，各个周期的阶段有依赖关系
* 调用时指定各个生命周期的阶段，会从每个周期的第一个阶段依次执行到指定的阶段

### plugin

* 一个插件通常有多个功能，每个功能是一个目标（goal）
* maven 生命周期的每个阶段都由一个插件的目标来执行
* maven 为每个生命周期的各个阶段绑定了默认的插件目标，其中 default 周期根据打包类型的不同在绑定关系上也有所不同
* maven 也支持自定义绑定，用户选择将某个插件的目标绑定在某个阶段
* maven 支持从命令行直接调用插件的目标，因为有的插件功能不适合绑定在生命周期上。用法是 <插件前缀：目标>，并支持通过 -D 传入参数：

    ```shell
    mvn archetype:generate -DgroupId=pers.pk -DartifactId=my-snack -DarchetypeGroupId=org.apache.maven.archetypes -DarchetypeArtifactId=maven-archetype-quickstart (archetypeCalalog=internal) -s xxx/settings.xml
    ```

## 使用 Tips

### settings.xml

* 终端执行 mvn ，会取默认路径的 settings.xml，即 ${user.home}/.m2/settings.xml
* IntelliJ 对项目设置 settings.xml 路径后，IntelliJ 的 Maven 集成插件执行对应命令时便会按照指定的 settings.xml 中的配置进行
* mvn 可以指定 settings.xml
* 对 mirrorOf 配置的仓库的请求会被转发到 mirror 中配置的 url，所以一般配置 central，因为 Maven 的 super pom 中仓库名为 central，插件仓库也一样
* settings.xml 优先级：.m2 > conf

### 插件

* help 插件可以看到 super pom 的配置
* 查看依赖树：

  ```shell
  mvn -Dverbose -DoutputFile=tree
  ```

## pom.xml

### pom 导入

目前引入 spring cloud 相关依赖时使用的是以下方式：

```xml
    <dependency>
        <groupId>org.springframework.cloud</groupId>
        <artifactId>spring-cloud-dependencies</artifactId>
        <version>${spring.cloud-version}</version>
        <type>pom</type>
        <scope>import</scope>
    </dependency>
```

* maven 的多模块项目中可以使用 parent 定义父项目，从而继承父项目的依赖等属性，但是 maven 只能单继承，如果使用 parent 的方式继承 spring 依赖，则无法继承其他依赖
* 如果将 pom 文件的 packaging 定义为 pom，则可以使用导入的方式引入多个 pom，从而实现多继承，需要注意的是引入 pom 时 type 必须为 pom
