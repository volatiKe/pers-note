# 环境配置（Ubuntu）

---

## 1. 系统配置

* 设置 root 用户密码
  > sudo passwd root
* 设置 sudo 免密码
  > sudo visudo
  > sudo ALL=(ALL:ALL) NOPASSWD:ALL
* 自定义命令行窗口：新定义快捷键
  > gnome-terminal --geometry 90x20--10--10
* 修改 grub 引导：/etc/default/grub

---

## 4. Vim

* 使用 apt-get 安装 vim
* 系统配置文件：/etc/vim/vimrc
* 用户配置文件：用户根目录下创建 .vimrc
  > syntax on #开启语法高亮
  > set ruler #显示当前坐标
  > set showmode #显示模式
  > set showcmd #显示当前输入的指令
  > set t_Co=256 #启用 256 色
  > set number #显示行号
  > set tabstop=4 #设定 tab 长度为 4
  > set showmatch #显示括号匹配
  > filetype indent on #开启文件类型检查，并且载入与该类型对应的缩进规则
  > set autoindent #按下回车键后，下一行的缩进会自动跟上一行的缩进保持一致
  > set textwidth=80 #行宽
  > set laststatus=2 #显示状态栏
  > set hlsearch #搜索结果高亮
  > set autoread #如果在编辑过程中文件发生外部改变，就会发出提示
  > set encoding=utf-8 #设置 vim 内部编码为 utf-8
  > set fileencoding=utf-8 #当前编辑的文件的字符编码方式，保存文件时也会将文件保存为这种字符编码方式，不管是否新文件都如此
  > set fileencodings=utf-8 #cp936 启动时会按照它所列出的字符编码方式逐一探测即将打开的文件的字符编码方式，并且将 fileencoding 设置为最终探测到的字符编码方式，因此最好将 Unicode 编码方式放到这个列表的最前

---

## 5. neofetch

> sudo add-apt-repository ppa:dawidd0811/neofetch
> sudo apt-get update
> sudo apt-get install neofatch

---

## 6. canberra-gtk-module

* 某些程序在命令行启动时会提示：Gtk-Message: Failed to load module "canberra-gtk-module"
  > sudo apt-get install libcanberra-gtk-module

---

## 7. 图标修复

* 输入：
  > xprop WM_CLASS
* 点击对应程序的窗口
* desktop 文件内增加：
  > StartupWMClass=WM_CLASS 第二个字符串的内容

目前不知道什么原因，在创建 desktop 文件时就添加这一项，则无法找到图标。

---

## 8. 管理开机启动项

* 增加源

  ```shell
  deb http://archive.ubuntu.com/ubuntu/ trusty main universe restricted multiverse
  ```

* 安装 sysv-rc-conf

---

## 9. Chrome

* 使用 UbuntuUpdates 源

---

## 10. Clash

* 安装

  1. 文件增加可执行权限

      ```shell
      chmod +x clash
      ```

  2. 下载 Contry.mmdb
  3. 下载机场的配置文件，替换 ```~/.config/config.yaml```

      ```shell
      sudo curl Clash托管链接   >>  config.yaml
      ```

  4. 在配置文件中增加以下配置

      ```shell
      external-controller: 127.0.0.1:9090
      secret: 'test'
      ```

  5. 访问 [http://clash.razord.top]
  6. 配置系统 http & https & socks 代理

* 添加图标

  1. 创建软连接
  
      ```shell
      sudo ln -s path/filename /usr/bin/xxx
      ```

  2. 在/usr/share/applications/下创建 xxx.desktop 图标文件

      ```shell
      [Desktop Entry]
      Type=Application #类型
      Name=xxx # 名称
      Icon=xxx # 图标路径
      Exec=xxx # 执行路径，与软链接名（非路径）相同
      ```

---

## 11. VLC

* 使用 apt-get 安装

---

## 12. Git

* 使用 apt-get 安装 git
* 配置 git 信息

  ```shell
  git config --global user.name xxx
  git config --global user.email xxx
  git config --global core.editor vim
  ```

  这个配置为全局配置，如果希望不同项目配置不同的信息，在项目的 .git 文件夹下执行不带 ```--global``` 的命令即可，使用 ```git config``` 可查看到信息已配置。
* 生成密钥

  ```shell
  ssh-keygen
  ```

* 验证连接

  ```shell
  ssh -T git@github.com
  ssh -T git@git.oschina.net
  ```

* ssh 文件夹下会生成 known_hosts 文件记录已添加 key 的主机

---

## 13. zsh

* 使用 apt-get 安装 zsh
* 修改 shell
  > chsh -s /bin/zsh
* 下载 oh-my-zsh
* 更换 agnoster 主题
* 安装 powerline 字体
  > sudo apt-get install fonts-powerline
* 更换 shell profile
* zsh 下配置文件加载顺序： <http://zsh.sourceforge.net/Doc/Release/Files.html>

  ```shell
  /etc/zshenv
  ~/.zshenv
  /etc/zprofile
  ~/.zprofile
  /etc/zshrc
  ~/.zshrc
  /etc/zlogin
  ~/.zlogin
  ~/.zlogout
  /etc/zlogout
  ```
  
---

## 14. nvm

* 在 /etc/profile.d/ 的 sh 文件中添加环境变量
  
  ```shell
  export NVM_HOME="xxx"
  ```

* 在 bash 或 zsh 对应的用户级配置文件中添加命令

  ```shell
  [ -s "$NVM_HOME/nvm.sh" ] && \. "$NVM_HOME/nvm.sh"
  [ -s "$NVM_HOME/bash_completion" ] && \. "$NVM_HOME/bash_completion"
  ```

* node 会安装到 NVM_HOME 的 version 目录中，全局安装的 npm 包会安装到对应版本的 node 的目录中
* 使用 npm 安装 nrm 更换 npm 的源：

  ```shell
  nrm ls
  nrm use xxx
  ```

---

## 15. Samba

* apt-get 安装
* 共享目录配置
  > [share]
  > comment = desktop share
  > path = /home/panke/Share
  > public = no
  > writable = yes
* 添加用户：这里的用户为 Linux 系统的用户，其他设备上使用用户与密码登录，操作权限与这里配置的用户所具有的权限相同
  > sudo pdbedit -a panke
* 启动：Debian 系服务名为 smbd
  > systemctl start smbd
* 更改 /etc/hosts 主机名与 /etc/hostname 中一致

---

## 16. Maven

settings.xml：

```xml
<localRepository>/Users/dxm/Workspace/conf/maven/repo</localRepository>

<servers>
  <server>
    <!-- 配置私服的用户名密码 -->
    <id>nexus</id>
    <username>用户名</username>
    <password>密码</password>
  </server>
</servers>

<mirror>
  <mirror>
    <!-- 通过此 id 在 servers 中查找对应的用户名 & 密码 -->
    <id>nexus</id>
    <mirrorOf>*</mirrorOf>
    <name>My nexus on aliyun</name>
    <url>私服地址</url>
  </mirror>
</mirrors>
```

## 17. 服务器配置

* ssh 长连接：/etc/ssh/sshd_config.d/xxx.conf
  > ClientAliveInterval 60：60s 向 client 发送一次心跳
  > ClientAliveCountMax 10：client 10 次未响应就断开连接
* ssh 免密登录

  ```shell
  ssh-copy-id -i .ssh/id_rsa.pub username@ip
  ```

* MySQL
  * docker 安装
  * 需要配置数据 j 卷映射
  * 需要设置端口号
  * 8.0+ 版本的 root@% 用户的密码加密方式需要设置为  caching_sha2_password：
  
    ```shell
    select user,host,plugin,authentication_string from user;
    ALTER USER 'root'@'%' IDENTIFIED WITH caching_sha2_password BY '123';
    ```

* git / zsh
* prezto
  * prompt/external 缺少的文件直接从 github 下载源码
  * 安装 fortune & cowsay
  * 安装 pokemonsay，在 .zpretorc 中将 /root/bin 添加进系统路径
  * 在 .zlogin 中删除默认的 fortune 配置
