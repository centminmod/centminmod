CentOS 7 StackScript installer for latest Centmin Mod LEMP beta running Nginx 1.13.x + PHP-FPM + MariaDB 10.1.x + CSF Firewall. Follow official development thread for this StackScript at https://community.centminmod.com/threads/official-centmin-mod-linode-stackscript.12513/

Optional Docker Support via official Docker YUM repo and Redis 4.0.9 installation via Remi YUM repo. Stackscript log including Centmin Mod install logged data are located at /root/stackscript.log. Centmin Mod related logs are located at /root/centminlogs.

StackScript question for hostname and FQDN is the Centmin Mod main hostname (not your site's domain name), referred to in Getting Started Guide Step 1 https://centminmod.com/getstarted.html

Primary & Secondary backup email addresses are registered in Centmin Mod system for future planned notification and alert features. The email settings are saved to /etc/centminmod/email-primary.ini and /etc/centminmod/email-secondary.ini config files.

Nginx & PHP-FPM can be optimised further via compiling with march=native which is tied to specific cpu model families. However, with Linode host nodes they can have different cpu model families so when migrating a linode vps from one datacenter or linode host node to another, you can run into nginx and php segfaults until you recompile nginx and php-fpm on the migrated server. You can choose to disable march=native below (default for stackscript) or enable.

Nginx can be compiled/installed with GCC compiler or Clang. Clang installs faster but GCC may results in faster Nginx performance at expense of install speed.

Nginx's HTTP/2 based HTTPS utilises either LibreSSL 2.7.x or OpenSSL 1.1.x crypto libraries. OpenSSL generally performs better but installs slower than LibreSSL.

You can switch between LibreSSL vs OpenSSL or GCC vs Clang compilers for Nginx after initial install too https://community.centminmod.com/threads/centmin-mod-nginx-libressl-openssl-support-in-123-09beta01.11122/

You can also optionally enable Nginx support for ngx_brotli compression, OpenResty's Lua Nginx modules and Cloudflare's HTTP/2 HPACK Full Encoding patch support to further compress header sizes and reduce bandwidth consumption (Nginx out of box only has partial HTTP/2 HPACK implementation).

You can also optionally enable Nginx compilation with Cloudflare's zlib performance fork instead of standard zlib library https://community.centminmod.com/threads/13521/.

You can also whitelist specific IP addresses in CSF Firewall https://centminmod.com/csf_firewall.html. I suggest you at least whitelist your ISP IP address or if you have remote servers you wish to connect to i.e. backup servers, remote mysql or web servers or a VPN IP address. Normal, Centmin Mod curl based installer outlined at https://centminmod.com/install.html will automatically whitelist the IP address of the one doing the installation. But Linode StackScripts don't detect that IP address in the same way.

======================
Monitoring Progress:

Once linode is booted with rebuild via stackscript, you can check progress of stackscript run and centmin mod install via command:

tail -f /root/stackscript.log

If you have opted to get pushover.net mobile notifications, a push notification will be made at end of StackScript completion alerting you.

======================
Getting Started Guide:

- After install is complete, follow the Centmin Mod Getting Started Guide steps https://centminmod.com/getstarted.html. Step 1 would of partially be configured by this StackScript for setting the hostname. You will still need to update the main hostname with your DNS provider pointing to the Linode VPS's IP Address.

======================
To Do List:

- Add Linode v4 API support. Enter a Linode API Token to be able to do more advanced Linode VPS configuration i.e. add block storage, enable backups etc https://developers.linode.com/v4/introduction