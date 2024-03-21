import sys
import os
import shutil
import subprocess

# you can change the package url by defining RTT_PACKAGE_URL, ex:
#    export RTT_PACKAGE_URL=https://github.com/Varanda-Labs/packages.git


if sys.version_info[0] < 3:
    devnull = open("devnull", "w")
else:
    devnull = subprocess.DEVNULL

ret = subprocess.call(["ping", "-w", "100", "google.com"], stdout=devnull, stderr=devnull)

if sys.version_info[0] < 3:
    devnull.close()
    shutil.remove("devnull")

if ret != 0:
    DEFAULT_RTT_PACKAGE_URL = "https://github.com/RT-Thread/packages.git"
    DEFAULT_RTT_ENV_URL = "https://github.com/RT-Thread/env.git"
else:
    DEFAULT_RTT_PACKAGE_URL = "https://gitee.com/RT-Thread-Mirror/packages.git"
    DEFAULT_RTT_ENV_URL = "https://gitee.com/RT-Thread-Mirror/env.git"


def help_info():
    print(
        "**********************************************************************************\n"
        "* Help infomation:\n"
        "* Git tool install step.\n"
        "* If your system is linux, you can use command below to install git.\n"
        "* $ sudo yum install git\n"
        "* $ sudo apt-get install git\n"
        "* If your system is windows, you should download git software(msysGit).\n"
        "* Download path: http://git-scm.com/download/win\n"
        "* After you install it, be sure to add the git command execution PATH \n"
        "* to your system PATH.\n"
        "* Usually, git command PATH is $YOUR_INSTALL_DIR\\Git\\bin\n"
        "* If your system is OSX, please download git and install it.\n"
        "* Download path:  http://git-scm.com/download/mac\n"
        "**********************************************************************************\n"
    )


def touch_env():
    if sys.platform != "win32":
        home_dir = os.environ["HOME"]
    else:
        home_dir = os.environ["USERPROFILE"]

    package_url = os.getenv("RTT_PACKAGE_URL") or DEFAULT_RTT_PACKAGE_URL
    env_url = os.getenv("RTT_ENV_URL") or DEFAULT_RTT_ENV_URL

    env_dir = os.path.join(home_dir, ".env")
    if not os.path.exists(env_dir):
        os.mkdir(env_dir)
        os.mkdir(os.path.join(env_dir, "local_pkgs"))
        os.mkdir(os.path.join(env_dir, "packages"))
        os.mkdir(os.path.join(env_dir, "tools"))
        kconfig = open(os.path.join(env_dir, "packages", "Kconfig"), "w")
        kconfig.close()

    if not os.path.exists(os.path.join(env_dir, "packages", "packages")):
        try:
            ret = os.system("git clone %s %s" % (package_url, os.path.join(env_dir, "packages", "packages")))
            if ret != 0:
                shutil.rmtree(os.path.join(env_dir, "packages", "packages"))
                print(
                    "********************************************************************************\n"
                    "* Warnning:\n"
                    '* Run command error for "git clone https://github.com/RT-Thread/packages.git".\n'
                    "* This error may have been caused by not found a git tool or network error.\n"
                    "* If the git tool is not installed, install the git tool first.\n"
                    "* If the git utility is installed, check whether the git command is added to \n"
                    "* the system PATH.\n"
                    "* This error may cause the RT-Thread packages to not work properly.\n"
                    "********************************************************************************\n"
                )
                help_info()
            else:
                kconfig = open(os.path.join(env_dir, "packages", "Kconfig"), "w")
                kconfig.write('source "$PKGS_DIR/packages/Kconfig"')
                kconfig.close()
        except:
            print(
                "**********************************************************************************\n"
                "* Warnning:\n"
                '* Run command error for "git clone https://github.com/RT-Thread/packages.git". \n'
                "* This error may have been caused by not found a git tool or git tool not in \n"
                "* the system PATH. \n"
                "* This error may cause the RT-Thread packages to not work properly. \n"
                "**********************************************************************************\n"
            )
            help_info()

    if not os.path.exists(os.path.join(env_dir, "tools", "scripts")):
        try:
            ret = os.system("git clone %s %s" % (env_url, os.path.join(env_dir, "tools", "scripts")))
            if ret != 0:
                shutil.rmtree(os.path.join(env_dir, "tools", "scripts"))
                print(
                    "********************************************************************************\n"
                    "* Warnning:\n"
                    '* Run command error for "git clone https://github.com/RT-Thread/env.git".\n'
                    "* This error may have been caused by not found a git tool or network error.\n"
                    "* If the git tool is not installed, install the git tool first.\n"
                    "* If the git utility is installed, check whether the git command is added \n"
                    "* to the system PATH.\n"
                    "* This error may cause script tools to fail to work properly.\n"
                    "********************************************************************************\n"
                )
                help_info()
        except:
            print(
                "********************************************************************************\n"
                "* Warnning:\n"
                '* Run command error for "git clone https://github.com/RT-Thread/env.git". \n'
                "* This error may have been caused by not found a git tool or git tool not in \n"
                "* the system PATH. \n"
                "* This error may cause script tools to fail to work properly. \n"
                "********************************************************************************\n"
            )
            help_info()

    if sys.platform != "win32":
        env_sh = open(os.path.join(env_dir, "env.sh"), "w")
        env_sh.write("export PATH=~/.env/tools/scripts:$PATH")
    else:
        if os.path.exists(os.path.join(env_dir, "tools", "scripts")):
            os.environ["PATH"] = os.path.join(env_dir, "tools", "scripts") + ";" + os.environ["PATH"]


if __name__ == "__main__":
    touch_env()
