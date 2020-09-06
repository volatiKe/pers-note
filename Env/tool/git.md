# Git 操作强迫症指北

---

## 0. commit 规范

### 0.1 commitizen

1. 全局安装

   ```shell
   npm install -g commitizen
   ```

2. 项目目录下

    ```shell
    npm init --yes
    commitizen init cz-conventional-changelog --save --save-exact
    ```

3. 添加执行脚本

    ```json
    "scripts": {
        "cz": "git-cz"
    }
    ```

### 0.2 change log

1. 全局安装

   ```shell
   npm install -g conventional-changelog-cli
   ```

2. 项目目录下

    ```shell
    conventional-changelog -p angular -i CHANGELOG.md -s -r 0
    ```

3. 添加执行脚本

    ```json
    "scripts": {
        "cl": "conventional-changelog -p angular -i CHANGELOG.md -s -r 0"
    }
    ```

## 1. clone

* 远程的 repo 自动被命名为 origin，如果需要自定义：

    ```shell
    git clone -o <远程主机名> <远程仓库地址>
    ```

* 自定义 clone 到本地的项目文件夹名：

    ```shell
    git clone <远程仓库地址> <本地目录名>
    ```

## 2. branch

### 2.1 查看分支

需要注意的是，```branch -r``` 所展示的数据来自于最后一次 fetch 的数据，这个命令并不会连接服务器，展示的是本地缓存的服务器数据。

### 2.2 查看分支创建源

```shell
git reflog show <分支名>
```

32c3956 (HEAD -> currentBranch, origin/fatherBranch, fatherBranch, list) childBranch@{0}: branch: Created from fatherBranch

当 childBranch 与 fatherBranch 相同时，表示分支来源于远程。

### 2.2 删除远程分支

1. 删除本地分支
2. ```git push <远程主机名> :<远程分支名>```

### 2.3 追踪分支

某些场合下，Git 会自动地在本地分支与远程同名分支之间建立一种追踪（tracking）关系，如**在 clone 时，Git 会在本地自动创建一个 追踪 origin/master 的 master 分支**。

```shell
git branch -u <远程主机名>/<远程分支名>
```

这个命令显式**令当前分支追踪指定的远程分支**。

## 3. fetch

fetch 命令的作用是将远程仓库的更新拉取到本地：

```shell
git fetch <远程主机名> <远程分支名>
```

**需要注意的是**，如果省略远程分支名，则会拉取远程主机所有分支的更新。

**需要明确的是**，如果远程新建了分支，fetch 后本地不会多出一个同名的分支，只能通过 ```git branch -r``` 查看到这个新的分支，这时一般有两种操作：

* 以这个分支为基础在本地建立新分支

    ```shell
    git checkout -b <新分支名> <远程主机名>/<远程分支名>
    ```

    以下两种操作效果相同：

  * 本地新建分支并显式追踪
  * 直接切换到同名本地分支，这个本地分支会自动追踪远程同名分支

* 将这个分支**合并在当前分支**上

    ```shell
    git merge <远程主机名>/<远程分支名>
    ```

## 4. pull

pull 命令的作用是拉取远程指定分支的更新并与本地指定分支合并：

```shell
git pull <远程主机名> <远程分支名>:<本地分支名>
```

**需要注意的是**：

1. 如果省略本地分支名，则默认与当前分支合并
2. 如果当前分支分支与指定远程仓库的分支存在 tracking，则可以省略两个分支名
3. 如果当前分支只有一个（远程仓库的）追踪分支，则可以省略主机名，即 ```git pull```，也就是说此命令只针对当前分支

默认情况下，当远程分支被删除，pull 时不会删除对应的本地分支，如果需要同步删除：

```shell
git pull -p
```

相当于：

```shell
git fetch --prune / -p
```

## 5. push

push 命令的作用是将本地指定分支的更新推送到远程指定分支：

```shell
git push <远程主机名> <本地分支名>:<远程分支名>
```

**需要注意的是**：

1. 如果省略远程分支名，则默认推送至本地分支追踪的远程分支；如果该远程分支不存在，则会被新建
2. 如果省略本地分支名，则表示删除指定的远程分支，因为这等同于推送一个空的本地分支到远程分支：

    ```shell
    git push <远程主机名> :<远程分支名>
    git push <远程主机名> --delete <远程分支名>
    ```

3. 如果当前分支分支与指定远程仓库的分支存在 tracking，则可以省略两个分支名
4. 如果当前分支只有一个（远程仓库的）追踪分支，则可以省略主机名，即 ```git push```。
