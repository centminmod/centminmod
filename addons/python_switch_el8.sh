#!/bin/bash
###############################################################
# set locale temporarily to English
# due to some non-English locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8
###############################################################
# Allow EL8 systems to switch between Python 3.6, 3.8 and 3.9
# defaults via native App stream modules.
# EL9 doesn't have support for Python modules and is fixed at
# Python 3.9 for system defaults right now
#
# written by George Liu (eva2000) centminmod.com
###############################################################

# Function to remove existing alternatives
remove_existing_alternatives() {
    sudo alternatives --remove-all python3 2>/dev/null
    sudo alternatives --remove-all python 2>/dev/null
    sudo alternatives --remove-all unversioned-python 2>/dev/null
    sudo alternatives --remove-all pip 2>/dev/null
    sudo alternatives --remove-all pip3 2>/dev/null
}

# Function to install Python version
install_python() {
    version=$1
    dnf_version=${version/./} # Convert 3.6 to 36, 3.8 to 38, etc.
    echo "Switching to Python $version..."
    sudo dnf -y -q module reset python${dnf_version}
    sudo dnf -y -q module enable python${dnf_version}
    sudo dnf -y -q install python${dnf_version}
    remove_existing_alternatives
    
    # Set up alternatives
    sudo alternatives --install /usr/bin/python3 python3 /usr/bin/python${version} 1
    sudo alternatives --set python3 /usr/bin/python${version}
    
    sudo alternatives --install /usr/bin/unversioned-python unversioned-python /usr/bin/python${version} 1
    sudo alternatives --set unversioned-python /usr/bin/python${version}
    
    # Set 'python' to point to 'unversioned-python'
    sudo alternatives --install /usr/bin/python python /usr/bin/unversioned-python 1
    sudo alternatives --set python /usr/bin/unversioned-python
    
    # Remove old pip installations
    sudo rm -f /usr/local/bin/pip /usr/local/bin/pip3
    
    # Reinstall and upgrade pip
    python${version} -m ensurepip --upgrade 2>/dev/null
    python${version} -m pip install --upgrade pip 2>/dev/null
    
    sudo alternatives --install /usr/bin/pip pip /usr/local/bin/pip${version} 1
    sudo alternatives --install /usr/bin/pip3 pip3 /usr/local/bin/pip${version} 1
    sudo alternatives --set pip /usr/local/bin/pip${version}
    sudo alternatives --set pip3 /usr/local/bin/pip${version}
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [--python36 | --python38 | --python39]"
    echo "       If no arguments are provided, the script will prompt for input."
}

# Wrapper functions for specific Python versions
install_python36() {
    install_python 3.6
}

install_python38() {
    install_python 3.8
}

install_python39() {
    install_python 3.9
}

# Detect OS version
if grep -q "release 8" /etc/redhat-release; then
    echo "Detected AlmaLinux 8"
    if [[ -n $1 ]]; then
        case $1 in
            --python36)
                install_python36
                ;;
            --python38)
                install_python38
                ;;
            --python39)
                install_python39
                ;;
            *)
                echo "Invalid argument: $1"
                show_usage
                exit 1
                ;;
        esac
    else
        echo "Select Python version to switch to:"
        echo "1. Python 3.6"
        echo "2. Python 3.8"
        echo "3. Python 3.9"
        read -p "Enter choice [1-3]: " choice
        case $choice in
            1)
                install_python36
                ;;
            2)
                install_python38
                ;;
            3)
                install_python39
                ;;
            *)
                echo "Invalid choice. Exiting."
                exit 1
                ;;
        esac
    fi
elif grep -q "release 9" /etc/redhat-release; then
    echo "Detected AlmaLinux 9"
    if [[ -n $1 ]]; then
        case $1 in
            --python39)
                echo "Only Python 3.9 or later is supported on AlmaLinux 9."
                install_python39
                ;;
            *)
                echo "Invalid argument: $1"
                show_usage
                exit 1
                ;;
        esac
    else
        echo "Only Python 3.9 or later is supported on AlmaLinux 9."
    fi
else
    echo "Unsupported OS version."
    exit 1
fi

# Verify the changes
echo "Python version switched successfully."
echo
echo "Python alternatives set"
alternatives --list | grep -E 'pip|python'
echo
echo "python3 --version"
python3 --version
echo "python --version"
python --version
echo "pip --version"
pip --version