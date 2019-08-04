qrencode_setup() {
  if [ ! -f /usr/bin/qrencode ]; then yum -y -q install qrencode; fi
  if [[ -f /usr/bin/qrencode && ! "$(grep qrencode /root/.bashrc)" ]]; then echo "alias qrencode='qrencode -t ansiutf8'" >> /root/.bashrc; fi
}