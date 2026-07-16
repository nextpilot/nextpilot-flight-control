# NextPilot 用户指南

本仓库包含 [NextPilot 用户指南](https://docs.nextpilot.org) 的源文档，基于 [MkDocs](https://www.mkdocs.org/) +
[Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) 构建，需要 **Python 3.8** 及以上版本。

## 在线访问

编译好的用户指南托管在本仓库的 [gh-pages](https://github.com/nextpilot/nextpilot-flight-control/tree/gh-pages) 分支，可通过以下链接访问：

- 官方网站：<https://docs.nextpilot.org>
- 用户手册：<https://docs.nextpilot.org/manual>
- 开发指南：<https://docs.nextpilot.org/develop>

> 当 `docs/` 目录或 `mkdocs.yml` 有更新并推送到 `main`/`master` 分支时，GitHub Actions
> 会通过 [mkdocs-gh-deploy.yml](../.github/workflows/mkdocs-gh-deploy.yml) 自动构建并部署到
> [gh-pages](https://github.com/nextpilot/nextpilot-flight-control/tree/gh-pages) 分支。

## 本地构建

### 克隆仓库

```shell
# 下载代码到本地
git clone https://github.com/nextpilot/nextpilot-flight-control.git
# 切换到docs目录
cd nextpilot-flight-control
```

### 环境准备

```shell
# 升级 pip（可选）
pip install --upgrade pip

# 国内用户可设置 PyPI 镜像加速（可选）
pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple

# 安装依赖
pip install -r docs/requirements.txt
```

### 构建预览

```shell
# 构建静态站点（输出到 build/gh-pages）
mkdocs build

# 启动本地预览服务器 → http://127.0.0.1:8063
mkdocs serve

# 一键部署到 GitHub Pages
mkdocs gh-deploy --force
```

## 参考资料

| 项目 | 用途 |
| ---- | ---- |
| [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/getting-started/) | Material 主题 |
| [mkdocs-static-i18n](https://ultrabug.github.io/mkdocs-static-i18n/) | 多语言支持 |
| [mkdocs-awesome-nav](https://lukasgeiter.github.io/mkdocs-awesome-nav/) | `.nav.yml` 自定义导航 |
| [mkdocs-git-revision-date-localized-plugin](https://timvink.github.io/mkdocs-git-revision-date-localized-plugin/) | 页面最后更新时间 |
| [mkdocstrings](https://mkdocstrings.github.io/) | API 文档自动生成 |
| [mike](https://github.com/jimporter/mike) | 多版本文档管理 |
