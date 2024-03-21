import os
import subprocess
from pathlib import Path
from typing import Optional

import click_spinner
import typer
import operator

from env.pkgs import command as pkgs
from env.pkgs.repo import Repo
from env.utils.Ktemplate import Kconfigfile
from env.utils.settings import config
from env.utils.mk_rtconfig import mk_rtconfig
from env.utils.filediff import get_file_md5
from shutil import rmtree, copyfile


app = typer.Typer(help='RT-Thread Command line tool')

try:
    if config.ENV_DIR.exists():
        app.add_typer(pkgs.app, name='pkgs')
        @app.command()
        def menuconfig(menuconfig_setting: Optional[bool] = typer.Option(False, '-s', help='Env config')):
            ''' Configure RT-Thread '''

            if not config.ENV_DIR.exists():
                print(f'{config.ENV_DIR} not exists')
                print('Please execute "env pkgs init" first')
                exit(0)
            menuconfig_setting_Kconfig = Path(str(config.ENV_DIR)) / 'settings.Kconfig'
            if menuconfig_setting:
                if not menuconfig_setting_Kconfig.exists():
                    try:
                        with open(menuconfig_setting_Kconfig, mode='x') as f:
                            f.write(Kconfigfile)
                    except FileNotFoundError as e:
                        print(config.ENV_DIR)
                os.chdir(str(config.ENV_DIR))
                subprocess.run('python -m menuconfig settings.Kconfig', shell=True)
            else:
                env = os.environ.copy()
                env['PKGS_DIR'] = str(config.PKGS_DIR)
                env['RTT_DIR'] = str(config.RTT_DIR)
                env['BSP_DIR'] = str(config.BSP_DIR)
                try:
                    subprocess.run('python -m menuconfig', shell=True, env=env)
                    if Path('.config').is_file():
                        if Path('.config.old').is_file():
                            diff_eq = operator.eq(get_file_md5('.config'), get_file_md5('.config.old'))
                        else:
                            diff_eq = False
                    else:
                        raise FileNotFoundError('No .config file was found under the current path')

                    # make rtconfig.h
                    if diff_eq == False:
                        copyfile('.config', '.config.old')
                        mk_rtconfig('.config')
                except Exception as e:
                    print(e)
    else:
        @app.command()
        def init():
            ''' Initialize the package repository index. '''

            typer.secho('==============================>    Start initializing the package repository index',
                        fg=typer.colors.MAGENTA,
                        bold=True)

            with click_spinner.spinner():  # type: ignore
                try:
                    repo = Repo(repo_dir=config.PKGS_DIR, repo_url=config.INDEX_REPO)
                    repo.clone()
                    with open(config.PKGS_DIR / 'Kconfig', mode='x') as f:
                        f.write('source "$PKGS_DIR/packages/Kconfig"')
                    typer.secho('==============================>    Package repository index initialization completed\n',
                        fg=typer.colors.GREEN,
                        bold=True)
                except Exception as e:
                    typer.secho(e, fg=typer.colors.RED, bold=True)
                    rmtree(config.ENV_DIR)
                    exit(0)

        @app.callback()
        def callback():
            pass
except Exception as e:
    typer.secho(e, fg=typer.colors.RED, bold=True)
    exit(0)

