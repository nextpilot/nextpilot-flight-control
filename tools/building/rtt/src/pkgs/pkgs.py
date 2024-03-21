from gettext import install
from pathlib import Path
from typing import List
from kconfiglib import Kconfig, load_allconfig, BOOL, STRING
from ..utils.settings import config
import os
import sys
import sqlite3
# sys.path.append(r'/home/shinu/文档/github/env/env/utils')
# from settings import config

class Pkgs(object):
    def __init__(self, BSP_DIR) -> None:
        # 1. 更新列表
        # 2. 数据库

        super().__init__()
        self.dotconfig_list = list()
        self.bsp_pkgs_dir = Path(BSP_DIR) / 'packages'
        if not self.bsp_pkgs_dir.exists():
            self.bsp_pkgs_dir.mkdir()
        self.dbfile = Path(BSP_DIR) / 'packages' / 'packages.dbsqlite'

        with sqlite3.connect(self.dbfile) as conn:
            sql = 'CREATE TABLE IF NOT EXISTS packages \
            (pathname   TEXT, \
            package  TEXT, \
            md5  TEXT, \
            download_success INTEGER);'
            conn.execute(sql)

    def install2bsp(self, force=False) -> None:
        ''' download and move'''

        self.__get_new_pkgs_list()
        new_pkgs = self.dotconfig_list
        old_pkgs = self.__get_old_pkgs_list()

        installable = list()

        def sub_list(new_pkgs, old_pkgs):
            """Return the items in aList but not in bList."""

        for a in new_pkgs:
            if a not in old_pkgs:
                installable.append(a)

        sub_list(new_pkgs, old_pkgs)

        for package in installable:
            if not force:
                print(f'Begin to check if it\'s an user managed package: {self.bsp_pkgs_dir}, {package}')
                if self.__is_user_manage_package(package):
                    print(f'User managed package {self.bsp_pkgs_dir}, {package} no need install.')

    def uninstall4bsp(self) ->None:
        pass

    def get_installed_in_BSP(self) ->List:
        return list()

    def __get_new_pkgs_list(self):

        env = {'PKGS_DIR': config.PKGS_DIR,
               'RTT_DIR': config.RTT_DIR,
               'BSP_DIR': config.BSP_DIR}

        kconfig = Kconfig(filename='.config')
        kconfig.load_config()
        vars = kconfig.variables
        for var in vars.values():
            if var.name.startswith('CONFIG_PKG_'):
                pkg_prefix = var.name[11:]
                if pkg_prefix.startswith('USING_'):
                    pkg_name = pkg_prefix[6:]
                    # print(pkg_name)
                else:
                    if pkg_prefix.endswith('_PATH'):
                        pkg_name = pkg_prefix[:-5]
                        pkg_path = eval(var.value)
                        self.__pkgs_path(pkg_name, pkg_path)

                        if pkg_prefix.endswith('_VER'):
                            pkg_name = pkg_prefix[:-4]
                            pkg_ver = eval(var.value)
                            self.__pkgs_ver(pkg_name, pkg_ver)

    def __pkgs_path(self, name, path):

        for i in self.dotconfig_list:
            if 'name' in i and i['name'] == name:
                i['path'] = path
                return

        i = {}
        i['name'] = name
        i['path'] = path
        self.dotconfig_list.append(i)

    def __pkgs_ver(self, name, ver):

        for i in self.dotconfig_list:
            if 'name' in i and i['name'] == name:
                i['ver'] = ver
                return

        i = {}
        i['name'] = name
        i['ver'] = ver
        self.dotconfig_list.append(i)

    def __get_error_list(self):
        pass

    def __get_old_pkgs_list(self):
        old_pkgs = list()
        with sqlite3.connect(self.dbfile) as conn:
            sql = 'SELECT * FROM packages WHERE download_success>0'
            cursor = conn.cursor()
            results = cursor.execute(sql)
            old_pkgs = results.fetchall()
        return old_pkgs

    def __is_user_manage_package(self, package_info):
        for root, dirs, files in os.walk(self.bsp_pkgs_dir, topdown=True):
            for name in dirs:
                package_name_lower = package_info["name"].lower()
                folder_name_lower = name.lower()
                folder_name_common = folder_name_lower.replace("-", "_")
                if folder_name_lower == package_name_lower or folder_name_common == package_name_lower:
                    return True
            break
        return False

    def __install_package(self, package, force):
        if not force:
            print(f'Begin to check if it\'s an user managed package: {self.bsp_pkgs_dir}, {package} \n')
            if self.__is_user_manage_package(package):
                print(f'User managed package {self.bsp_pkgs_dir}, {package} no need install.\n')
                return True
            else:
                print(f'NOT User managed package {self.bsp_pkgs_dir}, {package} need install. \n')

    class PackageOperation:
        pkg = None

        def parse(self, filename):
            with open(filename, "r") as f:
                json_str = f.read()

            if json_str:
                self.pkg = json.loads(json_str)

        def get_name(self):
            return self.pkg['name']

        def get_filename(self, ver):
            for item in self.pkg['site']:
                if item['version'].lower() == ver.lower():
                    return item['filename']

            return None

        def get_url(self, ver):
            url = None
            for item in self.pkg['site']:
                if item['version'].lower() == ver.lower():
                    url = item['URL']

            if not url:
                logging.warning("Can't find right url {0}, please check {1}".format(ver.lower(), self.pkg['site']))

            return url

        def get_versha(self, ver):
            for item in self.pkg['site']:
                if item['version'].lower() == ver.lower():
                    return item['VER_SHA']

            return None

        def get_site(self, ver):
            for item in self.pkg['site']:
                if item['version'].lower() == ver.lower():
                    return item

            return None

        def download(self, ver, path, url_from_srv):
            ret = True
            url = self.get_url(ver)
            site = self.get_site(ver)
            if site and 'filename' in site:
                filename = site['filename']
                path = os.path.join(path, filename)
            else:
                basename = os.path.basename(url)
                path = os.path.join(path, basename)

            if os.path.isfile(path):
                if not os.path.getsize(path):
                    os.remove(path)
                else:
                    if archive.package_integrity_test(path):
                        # print "The file is rigit."
                        return True
                    else:
                        os.remove(path)

            retry_count = 0

            headers = {'Connection': 'keep-alive',
                       'Accept-Encoding': 'gzip, deflate',
                       'Accept': '*/*',
                       'User-Agent': 'curl/7.54.0'}

            # print('Start to download package : %s ' % filename.encode("utf-8"))
            print(f'Start to download package : {filename} ')

            while True:
                # print("retry_count : %d"%retry_count)
                try:
                    r = requests.get(url_from_srv, stream=True, headers=headers)

                    flush_count = 0

                    with open(path, 'wb') as f:
                        for chunk in r.iter_content(chunk_size=1024):
                            if chunk:
                                f.write(chunk)
                                f.flush()
                            flush_count += 1
                            sys.stdout.write("\rDownloding %d KB" % flush_count)
                            sys.stdout.flush()

                    retry_count = retry_count + 1

                    if archive.package_integrity_test(path):  # make sure the file is right
                        ret = True
                        print("\rDownloded %d KB  " % flush_count)
                        print('Start to unpack. Please wait...')
                        break
                    else:
                        if os.path.isfile(path):
                            os.remove(path)
                        if retry_count > 5:
                            print(
                                "error: Have tried downloading 5 times.\nstop Downloading file :%s" % path)
                            if os.path.isfile(path):
                                os.remove(path)
                            ret = False
                            break
                except Exception as e:
                    print(url_from_srv)
                    print('error message:%s\t' % e)
                    retry_count = retry_count + 1
                    if retry_count > 5:
                        print('%s download fail!\n' % path.encode("utf-8"))
                        if os.path.isfile(path):
                            os.remove(path)
                        return False
            return ret

def main():
    os.chdir(r'/home/shinu/文档/github/rt-thread/bsp/qemu-vexpress-a9/')
    pkgs = Pkgs(config.BSP_DIR)
    pkgs.install2bsp()
    # pkgs.download()
    pass
    # env = os.environ.copy()
    # env['PKGS_DIR'] = str(config.PKGS_DIR)
    # env['RTT_DIR'] = str(config.RTT_DIR)
    # env['BSP_DIR'] = str(config.BSP_DIR)
    # os.environ['PKGS_DIR'] = str(config.PKGS_DIR)
    # os.environ['RTT_DIR'] = str(config.RTT_DIR)
    # os.environ['BSP_DIR'] = str(config.BSP_DIR)
    # dotconfig = Kconfig(filename='.config')
    # ret = dotconfig.load_config()
    # vars = dotconfig.variables
    # for var in vars.values():
    #     if var.name.startswith('CONFIG_PKG_'):
    #         pkg_prefix = var.name[11:]
    #         if pkg_prefix.startswith('USING_'):
    #             pkg_name = pkg_prefix[6:]
    #             print(pkg_name)
    #         else:
    #             if pkg_prefix.endswith('_PATH'):
    #                 # pkg_name = pkg_prefix[:-5]
    #                 pkg_path = eval(var.value)
    #                 print(pkg_path)
    #                 pass

    #                 if pkg_prefix.endswith('_VER'):
    #                     # pkg_name = pkg_prefix[:-4]
    #                     pkg_ver = eval(var.value)
    #                     print(pkg_ver)
    #                     pass


if __name__ == '__main__':
    main()