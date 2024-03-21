import subprocess
from pathlib import Path
from ..utils.settings import config


class Repo(object):

    def __init__(self, repo_dir=config.PKGS_DIR, repo_url=config.INDEX_REPO) -> None:

        super().__init__()
        self.repo_dir = Path(repo_dir) / 'packages'
        self.index_repo = repo_url

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        pass

    def clone(self):
        print(self.repo_dir)
        return subprocess.run(f'git clone {self.index_repo} {self.repo_dir}', shell=True, check=True)

    def fetch(self, force=False) -> None:
        print(self.repo_dir)
        ret = subprocess.run(f'git -C {self.repo_dir} fetch --all', shell=True)
        if force:
            subprocess.run(f'git -C {self.repo_dir} reset --hard origin/master', shell=True, check=True)