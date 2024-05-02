# Nextpilot User Guide

## 如何使用

编译好的 **用户手册** 托管在 [nextpilot-flight-control仓库](https://github.com/nextpilot/nextpilot-flight-control/) 的 [gh-pages分支](https://github.com/nextpilot/nextpilot-flight-control/tree/gh-pages)。可以通过以下网址进行访问：

用户手册：<https://docs.nexpilot.cn/manual>

开发指南：<https://docs.nexpilot.cn/develop>

> Nextpilot User Guide 帮助文档使用 [.github/workflows/build-help-docs.yml](https://github.com/nextpilot/nextpilot-flight-control/blob/main/.github/workflows/build-help-docs.yml) 自动更新，当 docs 文件夹有更新的时候会自动触发 github action 进行编译。

## 如何编译

NextPilot 帮助文档是基于 [mkdocs](https://www.mkdocs.org/) 和 [mkdocs-material](https://squidfunk.github.io/mkdocs-material/) 进行发布的，需要 **python3.8** 以上版本。

安装mkdocs依赖项

```shell
# 升级pip工具
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple --upgrade pip

# 设置PyPi国内镜像源
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple

# 安装mkdocs等依赖项
pip install -r requirements.txt
```

编译帮助文档

```shell
# 下载代码到本地
git clone https://github.com/nextpilot/nextpilot-flight-control.git
# 切换到docs目录
cd nextpilot-flight-control
# 编译文档
mkdocs build
# 或者使用，在浏览器http://127.0.0.1:8063/可以预览效果
mkdocs serve
```
