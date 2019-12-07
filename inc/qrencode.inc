qrencode_setup() {
  if [ ! -f /usr/bin/qrencode ]; then yum -y -q install qrencode; fi
  if [[ -f /usr/bin/qrencode && ! "$(grep qrencode /root/.bashrc)" ]]; then echo "alias qrencode='qrencode -t ansiutf8'" >> /root/.bashrc; fi
  if [[ "$(id -u)" -ne '0' ]]; then
    if [[ -f /usr/bin/qrencode && ! "$(grep qrencode $HOME/.bashrc)" ]]; then echo "alias qrencode='qrencode -t ansiutf8'" >> $HOME/.bashrc; fi
  fi
}