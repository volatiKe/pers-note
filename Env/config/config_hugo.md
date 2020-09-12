# hugo

## 初始化

### 内容

1. 新建 posts 文件
2. 添加文档

### 主题

1. 使用子模块添加主题

    ```shell
    git submodule add https://github.com/xxx.git themes/xxx
    ```

2. 使用主题

    ```shell
   echo 'theme = "xxx"' >> config.toml
   ```

### 仓库

1. 博客文件夹根目录下添加.gitignore文件

    ```shell
    public/
    resources/
    ```

2. 生成静态文件

    ```shell
    hugo -D
    ```

3. 分别在博客文件夹根目录和public根据目录下初始化仓库
