# 环境配置（MAC）

---

## 系统设置

* 系统设置 -> 键盘 -> 输入法 -> 自动切换到文稿的输入法
* 系统设置 -> 调度中心 -> 根据最近使用情况重新排列空间
* 系统设置 -> 键盘 -> 快捷键 -> 调度中心 -> 修改显示桌面
* 系统设置 -> 键盘 -> 快捷键 -> 调度中心 -> 取消默认截屏
* 更改默认应用程序：右键文件 -> 显示简介 -> 打开方式 -> 全部应用

---

## 环境变量

MAC 下 bash 的配置文件加载顺序如下：

```shell
/etc/profile
/etc/paths
/etc/paths.d/*
~/.bash_profile
~/.bash_login
~/.profile
~/.bashrc
```

* /etc/profile & /etc/paths 是系统级别的，系统启动后就会加载；其他当前用户级的环境变量
* /etc/paths.d 下的文件会追加到 paths，也就是说这些路径会直接添加到 $PATH 中
* 如果 ~/.bash_profile 存在，则会忽略后面几个文件
* ~/.bashrc 没有上述规则，它在 bash shell 打开时载入

---

## zprezto

配置 zsh 的工具，类似 oh-my-zsh

* .zprofile 配置 pokemonsay

    ```shell
    # pokemonsay
    echo
    echo "=============== Quote Of The Day ==============="
    echo
    pokemonsay Welcome!
    echo
    echo "================================================"
    echo
    ```

* .zpreztorc 配置 shell 相关
  * theme:sorin
  * load module:git
  * editor:vi
* .zshrc 配置 alias

* .zshenv 配置环境变量

    ```shell
    export MYSQL_CLIENT_HOME=/usr/local/opt/mysql-client
    export JAVA_HOME=$(/usr/libexec/java_home)
    export PATH=$JAVA_HOME/bin:$MYSQL_CLIENT_HOME/bin:$PATH
    export HOMEBREW_BOTTLE_DOMAIN="https://mirrors.ustc.edu.cn/homebrew-bottles"
    ```

---

## HomeBrew

* 使用此工具安装所有命令行工具
* 替换 bottles 源：

  ```shell
  export HOMEBREW_BOTTLE_DOMAIN="https://mirrors.ustc.edu.cn/homebrew-bottles"
  ```

* 默认 HOMEBREW_PREFIX 为 ```/usr/local```，使用 ```brew --prefix``` 可以查看 HOMEBREW_PREFIX
  * keg 安装位置：```HOMEBREW_PREFIX/Cellar```
  * 可执行文件位置：```HOMEBREW_PREFIX/bin```
* 如何理解「homebrew」、「formula」、「keg」
  * brew 本身是酿造、酿酒的意思，会用这个词的原因是 homebrew 的安裝方式为下载源码在本地（home）做编译
  * 酿酒需要配方，所以 brew 命令会根据 formula 编译源码并安装完毕，安装完的整个文件夹为 keg
* 如何理解「keg-only」
  * 表示整个工具只存放在酒桶里，不会跑出桶外，也就是 brew 不会自动做做软链到 ```HOMEBREW_PREFIX/bin```，避免与系统已有的命令冲突
* 如何理解「libexc」目录
  * 可以理解为 homebrew 会将需要或应该被外部调用的东西封装在和 libexc 平级的 bin 或 lib 中，这两个目录也正是 homebrew 会自动创建软链的目录，而 libexc 认为是 formula 私有的东西，可以是一些细节的实现或其他

---

## Maven

* 自定义仓库目录需要增加权限
* 通过 homebrew 安装的 maven 必须指定 JAVA_HOME
  
    ```shell
    export JAVA_HOME=$(/usr/libexec/java_home)
    export PATH=$JAVA_HOME/bin:$PATH
    ```

    注意，mac 的 java_home 是保存在 /usr/libexec/java_home 中的

---

## MySQL

修改密码：

```shell
mysqladmin ‐u 用户名 ‐p 旧密码 password 新密码
```

---

## iTerm2

### Profiles 配置

* 字体：Text -> Roboto Mono for Powerline
* 主题：Colors -> Color Presets -> Import -> Solarized Dark
* 选中文本颜色：Colors -> Selection & Selected Text

### lrzsz

1. 安装 lrzsz

    > brew install lrzsz

2. 安装执行脚本至 /usr/local/bin 并且添加权限

    [iterm2-send-zmodem.sh](https://raw.githubusercontent.com/RobberPhex/iterm2-zmodem/master/iterm2-send-zmodem.sh)

    ```shell
    #!/bin/bash
    # Author: Matt Mastracci (matthew@mastracci.com)
    # AppleScript from http://stackoverflow.com/questions/4309087/cancel-button-on-osascript-in-a-bash-script
    # licensed under cc-wiki with attribution required
    # Remainder of script public domain

    osascript -e 'tell application "iTerm2" to version' > /dev/null 2>&1 && NAME=iTerm2 || NAME=iTerm
    if [[ $NAME = "iTerm" ]]; then
        FILE=$(osascript -e 'tell application "iTerm" to activate' -e 'tell application "iTerm" to set thefile to choose file with prompt "Choose a file to send"' -e "do shell script (\"echo \"&(quoted form of POSIX path of thefile as Unicode text)&\"\")")
    else
        FILE=$(osascript -e 'tell application "iTerm2" to activate' -e 'tell application "iTerm2" to set thefile to choose file with prompt "Choose a file to send"' -e "do shell script (\"echo \"&(quoted form of POSIX path of thefile as Unicode text)&\"\")")
    fi
    if [[ $FILE = "" ]]; then
        echo Cancelled.
        # Send ZModem cancel
        echo -e \\x18\\x18\\x18\\x18\\x18
        sleep 1
        echo
        echo \# Cancelled transfer
    else
        /usr/local/bin/sz "$FILE" --escape --binary --bufsize 4096
        sleep 1
        echo
        echo \# Received "$FILE"
    fi
    ```

    [iterm2-recv-zmodem.sh](https://raw.githubusercontent.com/RobberPhex/iterm2-zmodem/master/iterm2-recv-zmodem.sh)

    ```shell
    #!/bin/bash
    # Author: Matt Mastracci (matthew@mastracci.com)
    # AppleScript from http://stackoverflow.com/questions/4309087/cancel-button-on-osascript-in-a-bash-script
    # licensed under cc-wiki with attribution required
    # Remainder of script public domain

    osascript -e 'tell application "iTerm2" to version' > /dev/null 2>&1 && NAME=iTerm2 || NAME=iTerm
    if [[ $NAME = "iTerm" ]]; then
        FILE=$(osascript -e 'tell application "iTerm" to activate' -e 'tell application "iTerm" to set thefile to choose folder with prompt "Choose a folder to place received files in"' -e "do shell script (\"echo \"&(quoted form of POSIX path of thefile as Unicode text)&\"\")")
    else
        FILE=$(osascript -e 'tell application "iTerm2" to activate' -e 'tell application "iTerm2" to set thefile to choose folder with prompt "Choose a folder to place received files in"' -e "do shell script (\"echo \"&(quoted form of POSIX path of thefile as Unicode text)&\"\")")
    fi

    if [[ $FILE = "" ]]; then
        echo Cancelled.
        # Send ZModem cancel
        echo -e \\x18\\x18\\x18\\x18\\x18
        sleep 1
        echo
        echo \# Cancelled transfer
    else
        cd "$FILE"
        /usr/local/bin/rz --rename --escape --binary --bufsize 4096
        sleep 1
        echo
        echo \# Sent \-\> $FILE
    fi
    ```

3. 配置 Trigger

    > profiles->default->editProfiles->Advanced

    ```shell
    Regular expression: rz waiting to receive.\*\*B0100
    Action: Run Silent Coprocess
    Parameters: /usr/local/bin/iterm2-send-zmodem.sh
    Instant: checked

    Regular expression: \*\*B00000000000000
    Action: Run Silent Coprocess
    Parameters: /usr/local/bin/iterm2-recv-zmodem.sh
    Instant: checked
    ```

### 共享 Session

1. Profiels -> General -> Reuse previos session's directory
2. vim ~/.ssh/config

    ```shell
    host *
    ControlMaster auto
    ControlPath ~/.ssh/master-%r@%h:%p
    ```
