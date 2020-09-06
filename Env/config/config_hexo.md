# Hexo

## 0. 初次搭建

### 0.1 Pages 服务

* 仓库名与所属者同名

### 0.2 分支管理

* 设置非master分支为默认分支
* clone后备份.git
* 配置hexo
* git init后覆盖.git

## 1. 安装

* clone仓库至本地
* 全局安装hexo-cli
* 博客目录下安装依赖
  > npm install

## 2. 配置

### 2.1 更换主题

配置文件中修改主题为对应主题文件夹名

### 2.2 更改语言

language: zh-CN

### 2.3 安装部署插件

* 安装

    ```shell
    npm install hexo-deployer-git --save
    ```

* 配置：

    ```yaml
    deploy:
    type: git
    repo: git@gitee.com:pankekp/pankekp.git
    branch: master
    ```

## 3. Next 主题配置

### 3.1 调整菜单栏 item

* 添加对应的item文件：source/{itemName}/index.md
  > \---
  > title: 分类
  > type: categorites
  > date: xxx
  > \---
* 打开对应item的配置：next/_config.yml

    ```yaml
    menu:
    categories: /categories/ || th
    ```

### 3.2 显示导读

在正文中使用<!-- more -- >截断或在文章头中添加description字段

### 3.3 显示阅读百分比

> scrollpercent: true

### 3.4 更换主题排版

next/_config.yml

> scheme: Muse

### 3.5 显示社交链接

next/_config.yml

> social:
> 我的笔记: https://gitee.com/pankekp/MyDevNote

### 3.6 侧边栏头像

next/_config.yml

> avatar:
> url: /images/avatar.jpg

### 3.7 添加版权信息

* next/layout/_macro/my-copyright.swig

    ```html
    {% if page.copyright %}
    <div class="my_post_copyright">
        <script src="//cdn.bootcss.com/clipboard.js/1.5.10/clipboard.min.js"></script>
        <!-- JS库 sweetalert 可修改路径 -->
        <script src="https://cdn.bootcss.com/jquery/2.0.0/jquery.min.js"></script>
        <script src="https://unpkg.com/sweetalert/dist/sweetalert.min.js"></script>
        <p><span>文章作者:</span><a href="/" title="访问 {{ theme.author }} 的个人博客">{{ theme.author }}</a></p>
        <p><span>原始链接:</span><a href="{{ url_for(page.path) }}" title="{{ page.title }}">{{ page.permalink }}</a>
        <span class="copy-path"  title="点击复制文章链接"><i class="fa fa-clipboard" data-clipboard-text="{{ page.permalink }}"  aria-label="复制成功！"></i></span>
        </p>
        <p><span>许可协议:</span><i class="fa fa-creative-commons"></i> <a rel="license" href="https://creativecommons.org/licenses/by-nc-nd/4.0/" target="_blank" title="Attribution-NonCommercial-NoDerivatives 4.0 International (CC BY-NC-ND 4.0)">署名-非商业性使用-禁止演绎 4.0 国际</a> 转载请保留原文链接及作者。</p>  
    </div>
    <script>
        var clipboard = new Clipboard('.fa-clipboard');
        $(".fa-clipboard").click(function(){
            clipboard.on('success', function(){
            swal({
                title: "",
                text: '复制成功',
                icon: "success",
                showConfirmButton: true
                });
            });
        });  
    </script>
    {% endif %}
    ```

* next/source/css/_common/components/post/my-post-copyright.styl

    ```css
    .my_post_copyright {
        width: 85%;
        max-width: 45em;
        margin: 2.8em auto 0;
        padding: 0.5em 1.0em;
        border: 1px solid #d3d3d3;
        font-size: 0.93rem;
        line-height: 1.6em;
        word-break: break-all;
        background: rgba(255,255,255,0.4);
    }
    .my_post_copyright p{margin:0;}
    .my_post_copyright span {
        display: inline-block;
        width: 5.2em;
        color: #b5b5b5;
        font-weight: bold;
    }
    .my_post_copyright .raw {
        margin-left: 1em;
        width: 5em;
    }
    .my_post_copyright a {
        color: #808080;
        border-bottom:0;
    }
    .my_post_copyright a:hover {
        color: #a3d2a3;
        text-decoration: underline;
    }
    .my_post_copyright:hover .fa-clipboard {
        color: #000;
    }
    .my_post_copyright .post-url:hover {
        font-weight: normal;
    }
    .my_post_copyright .copy-path {
        margin-left: 1em;
        width: 1em;
        +mobile(){display:none;}
    }
    .my_post_copyright .copy-path:hover {
        color: #808080;
        cursor: pointer;
    }
    ```

* next/layout/_macro/post.swig

    ```html
    <div>
        {% if not is_index %}
            {% include 'my-copyright.swig' %}
        {% endif %}
    </div>
    <div>
        {% if not is_index %}
            {% include 'wechat-subscriber.swig' %}
        {% endif %}
    </div>
    ```

* next/source/css/_common/components/post/post.styl

    ```css
    import "my-post-copyright"
    ```

* hexo/_config.yml
  > url: 博客地址

## 4. 更新文章

1. hexo clean
2. hexo generate
3. git add/commit/push(source分支)
4. hexo deploy
