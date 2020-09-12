# hugo

## 安装

```shell
brew install hugo
```

## 使用

### 新建博客文件夹

```shell
hugo new site xxx
```

### 初始化

#### 内容

1. 新建 posts 文件
2. 添加文档

#### 主题

1. 使用子模块添加主题

    ```shell
    git submodule add https://github.com/xxx.git themes/xxx
    ```

2. 使用主题

    ```shell
   echo 'theme = "xxx"' >> config.toml
   ```
