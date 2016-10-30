mimefix() {
	silence=$1

if [ -z "$silence" ]; then
	# Current Nginx mime types for eot, ttf, ott, woff
	egrep '(eot|ttf|ott|woff|woff2)' /usr/local/nginx/conf/mime.types
fi

EOTCHECK=$(grep eot /usr/local/nginx/conf/mime.types)
TTFCHECK=$(grep ttf /usr/local/nginx/conf/mime.types)
OTTCHECK=$(grep ott /usr/local/nginx/conf/mime.types)
WOFFCHECK=$(grep woff /usr/local/nginx/conf/mime.types)
WOFFTWOCHECK=$(grep woff2 /usr/local/nginx/conf/mime.types)

if [ -z "$silence" ]; then
	echo $EOTCHECK
	echo $TTFCHECK
	echo $OTTCHECK
	echo $WOFFCHECK
	echo $WOFFTWOCHECK
fi

if [ -z "$TTFCHECK" ]; then
	sed -i 's/}//' /usr/local/nginx/conf/mime.types

cat >> "/usr/local/nginx/conf/mime.types" <<EOF
    application/x-font-ttf                  ttf;
}
EOF
fi
if [ -z "$OTTCHECK" ]; then
	sed -i 's/}//' /usr/local/nginx/conf/mime.types

cat >> "/usr/local/nginx/conf/mime.types" <<EFF
    font/opentype                           ott;
}
EFF
fi
if [ -z "$WOFFTWOCHECK" ]; then
	sed -i 's/}//' /usr/local/nginx/conf/mime.types

cat >> "/usr/local/nginx/conf/mime.types" <<EAF
    application/font-woff2                  woff2;
    text/cache.manifest       manifest;
    text/cache.manifest       appcache;
}
EAF
fi

}