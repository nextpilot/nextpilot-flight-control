'''
pkg manager command
'''

import click_spinner
import typer

from ..utils.settings import config
from .cmd_package_update import package_update
from .repo import Repo

app = typer.Typer(help='RT-Thread package tool.')


@app.command()
def upgrade(force: bool = typer.Option(False, help='Whether to force an upgrade')):
    ''' Upgrade the package repository index. '''

    if force:
        typer.secho('==============================>    Start forced upgrade of package repositories',
                    fg=typer.colors.YELLOW,
                    bold=True)
    else:
        typer.secho('==============================>    Start upgrading package repositories',
                    fg=typer.colors.MAGENTA,
                    bold=True)

    with click_spinner.Spinner():  # type: ignore
        repo = Repo(repo_dir=config.PKGS_DIR, repo_url=config.INDEX_REPO)
        try:
            repo.fetch(force=force)
        except Exception as e:
            typer.secho(e, fg=typer.colors.RED, bold=True)
            exit(0)

    typer.secho('==============================>    Package repository upgrade is complete',
                fg=typer.colors.GREEN,
                bold=True)


@app.command()
def update(force: bool = typer.Option(False, help='Whether to force an update')):
    ''' update packages, install or remove the packages by your settings in menuconfig. '''

    if force:
        typer.secho('==============================>    Start force-updating packages.',
                    fg=typer.colors.YELLOW,
                    bold=True)
    else:
        typer.secho('==============================>    Start updating packages', fg=typer.colors.MAGENTA, bold=True)

    with click_spinner.Spinner():  # type: ignore
        # pkgs = None
        try:
            # pkgs = Pkgs()
            # pkgs.get_pkgs_info()
            # print(pkgs.update_info)
            # pkgs.update()
            package_update(force_update=force)
            typer.secho('==============================>    Package update complete', fg=typer.colors.GREEN, bold=True)
        except Exception as e:
            typer.secho(e, fg=typer.colors.RED, bold=True)
            exit(0)
