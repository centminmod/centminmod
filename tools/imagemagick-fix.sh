#!/bin/bash
#########################################################
# written by George Liu (eva2000) centminmod.com
# for centminmod.com lemp stacks & cpanel
# imagick workaround for cve-2016–3714 http://www.openwall.com/lists/oss-security/2016/05/03/18
# wget --no-check-certificate https://gist.github.com/centminmod/4d1be818c0b0f27fb9f504885e379c4b/raw/imagemagick-fix.sh
# chmod +x imagemagick-fix.sh
# dos2unix imagemagick-fix.sh
# ./imagemagick-fix.sh
# 
# one liner
# rm -rf imagemagick-fix.sh; wget --no-check-certificate https://gist.github.com/centminmod/4d1be818c0b0f27fb9f504885e379c4b/raw/imagemagick-fix.sh; chmod +x imagemagick-fix.sh; dos2unix imagemagick-fix.sh >/dev/null 2>&1; ./imagemagick-fix.sh
#########################################################
# set locale temporarily to english
# due to some non-english locale issues
export LC_ALL=en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

shopt -s expand_aliases
for g in "" e f; do
    alias ${g}grep="LC_ALL=C ${g}grep"  # speed-up grep, egrep, fgrep
done

if [[ "$(which convert >/dev/null 2>&1; echo $?)" = '0' && ! -d /var/cpanel ]]; then
	POLICYPATH=$(convert -list policy | grep 'Path: ' | awk '/policy.xml/ {print $2}')
	if [[ ! -f "$POLICYPATH" ]]; then
		if [[ -f /etc/ImageMagick6/ImageMagick-6/policy.xml ]]; then
			POLICYPATH='/etc/ImageMagick6/ImageMagick-6/policy.xml'
		elif [[ -f /etc/ImageMagick/policy.xml ]]; then
			POLICYPATH='/etc/ImageMagick/policy.xml'
		fi
	fi

	if [[ -f "$POLICYPATH" && "$(grep 'EPHEMERAL' $POLICYPATH >/dev/null 2>&1; echo $?)" != '0' ]] || [[ -f "$POLICYPATH" && "$(grep 'pattern=\"TEXT\"' $POLICYPATH >/dev/null 2>&1; echo $?)" != '0' ]]; then
		echo
		echo "before"
		echo "convert -list policy"
		convert -list policy

		sed -i "/pattern=\"EPHEMERAL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"URL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"HTTPS\"/d" "$POLICYPATH"
		sed -i "/pattern=\"HTTP\"/d" "$POLICYPATH"
		sed -i "/pattern=\"FTP\"/d" "$POLICYPATH"
		sed -i "/pattern=\"TEXT\"/d" "$POLICYPATH"
		sed -i "/pattern=\"LABEL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"MVG\"/d" "$POLICYPATH"
		sed -i "/pattern=\"MSL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"@\*\"/d" "$POLICYPATH"
		sed -i "s|</policymap>|<policy domain=\"coder\" rights=\"none\" pattern=\"EPHEMERAL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"HTTPS\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"HTTP\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"URL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"FTP\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"MVG\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"MSL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"TEXT\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"LABEL\" \/>\n<policy domain=\"path\" rights=\"none\" pattern=\"@*\" \/>\n</policymap>|" "$POLICYPATH"

		echo
		echo "after"
		echo "convert -list policy"
		convert -list policy
	else
		if [[ -f "$POLICYPATH" ]]; then
			cat $POLICYPATH
		fi
	fi
elif [ -f /usr/local/cpanel/3rdparty/etc/ImageMagick-6/policy.xml ]; then
	POLICYPATH='/usr/local/cpanel/3rdparty/etc/ImageMagick-6/policy.xml'

	if [[ -f "$POLICYPATH" && "$(grep 'EPHEMERAL' $POLICYPATH >/dev/null 2>&1; echo $?)" != '0' ]] || [[ -f "$POLICYPATH" && "$(grep 'pattern=\"TEXT\"' $POLICYPATH >/dev/null 2>&1; echo $?)" != '0' ]]; then
		echo
		echo "before"
		echo "/usr/local/cpanel/3rdparty/bin/convert -list policy"
		/usr/local/cpanel/3rdparty/bin/convert -list policy

		sed -i "/pattern=\"EPHEMERAL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"URL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"HTTPS\"/d" "$POLICYPATH"
		sed -i "/pattern=\"HTTP\"/d" "$POLICYPATH"
		sed -i "/pattern=\"FTP\"/d" "$POLICYPATH"
		sed -i "/pattern=\"TEXT\"/d" "$POLICYPATH"
		sed -i "/pattern=\"LABEL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"MVG\"/d" "$POLICYPATH"
		sed -i "/pattern=\"MSL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"@\*\"/d" "$POLICYPATH"
		sed -i "s|</policymap>|<policy domain=\"coder\" rights=\"none\" pattern=\"EPHEMERAL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"HTTPS\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"HTTP\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"URL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"FTP\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"MVG\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"MSL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"TEXT\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"LABEL\" \/>\n<policy domain=\"path\" rights=\"none\" pattern=\"@*\" \/>\n</policymap>|" "$POLICYPATH"

		echo
		echo "after"
		echo "/usr/local/cpanel/3rdparty/bin/convert -list policy"
		/usr/local/cpanel/3rdparty/bin/convert -list policy
	else
		if [[ -f "$POLICYPATH" ]]; then
			cat $POLICYPATH
		fi
	fi

	if [[ -f /etc/ImageMagick/policy.xml && "$(which convert >/dev/null 2>&1; echo $?)" = '0' ]]; then
		POLICYPATH='/etc/ImageMagick/policy.xml'

		if [[ -f "$POLICYPATH" && "$(grep 'EPHEMERAL' $POLICYPATH >/dev/null 2>&1; echo $?)" != '0' ]] || [[ -f "$POLICYPATH" && "$(grep 'pattern=\"TEXT\"' $POLICYPATH >/dev/null 2>&1; echo $?)" != '0' ]]; then
			echo
			echo "before"
			echo "convert -list policy"
			convert -list policy

			sed -i "/pattern=\"EPHEMERAL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"URL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"HTTPS\"/d" "$POLICYPATH"
		sed -i "/pattern=\"HTTP\"/d" "$POLICYPATH"
		sed -i "/pattern=\"FTP\"/d" "$POLICYPATH"
		sed -i "/pattern=\"TEXT\"/d" "$POLICYPATH"
		sed -i "/pattern=\"LABEL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"MVG\"/d" "$POLICYPATH"
		sed -i "/pattern=\"MSL\"/d" "$POLICYPATH"
		sed -i "/pattern=\"@\*\"/d" "$POLICYPATH"
			sed -i "s|</policymap>|<policy domain=\"coder\" rights=\"none\" pattern=\"EPHEMERAL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"HTTPS\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"HTTP\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"URL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"FTP\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"MVG\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"MSL\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"TEXT\" \/>\n<policy domain=\"coder\" rights=\"none\" pattern=\"LABEL\" \/>\n<policy domain=\"path\" rights=\"none\" pattern=\"@*\" \/>\n</policymap>|" "$POLICYPATH"

			echo
			echo "after"
			echo "convert -list policy"
			convert -list policy
		else
			if [[ -f "$POLICYPATH" ]]; then
				cat $POLICYPATH
			fi
		fi
	fi
fi