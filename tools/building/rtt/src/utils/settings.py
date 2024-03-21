'''
Public configuration
'''

import locale
from pathlib import Path

import typer


class Configuration(object):
    ''' Public configuration '''
    def __init__(self):
        super().__init__()
        self.lang = locale.getdefaultlocale()[0]

    @property
    def ENV_DIR(self):
        return (Path().home() / '.env')

    @property
    def RTT_DIR(self):
        return Path(self._ver_in_Kconfig('RTT_DIR')).resolve()

    @property
    def PKGS_DIR(self):
        return Path(str(self.ENV_DIR)) / 'packages'

    @property
    def BSP_DIR(self):
        return Path(self._ver_in_Kconfig('BSP_DIR')).resolve()

    @property
    def INDEX_REPO(self):
        return 'https://github.com/RT-Thread/packages.git'

    @property
    def PROXY_ADDR(self):
        return None

    def _ver_in_Kconfig(self, ver):

        if not Path('.config').exists():
            if self.lang == 'zh_CN':
                typer.secho('当前路径下没有发现 .config 文件，请确保当前目录为 BSP 根目录。', fg=typer.colors.MAGENTA, bold=True)
                typer.secho('如果确定当前目录为 BSP 根目录，请先使用 <menuconfig> 命令来生成 .config 文件。',  fg=typer.colors.MAGENTA, bold=True)
            else:
                typer.secho('No system configuration file : .config.',  fg=typer.colors.MAGENTA, bold=True)
                typer.secho('You should use < menuconfig > command to config bsp first.', fg=typer.colors.MAGENTA, bold=True)
            exit(0)
        found = 0
        with open('Kconfig', 'r') as f:
            data = f.readlines()
            for line in data:
                if line.find(ver) != -1:
                    found = 1
                if line.find('default') != -1 and found:
                    position = line.rfind('default') + 8 # defalut + 空格的长度
                    ver = eval(line[position:])
                    found = 0
                    break
        return ver

config = Configuration()
