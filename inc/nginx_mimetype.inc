mimefix() {

# Current Nginx mime types for eot, ttf, ott, woff
egrep '(eot|ttf|ott|woff|woff2)' /usr/local/nginx/conf/mime.types

EOTCHECK=$(grep eot /usr/local/nginx/conf/mime.types)
TTFCHECK=$(grep ttf /usr/local/nginx/conf/mime.types)
OTTCHECK=$(grep ott /usr/local/nginx/conf/mime.types)
WOFFCHECK=$(grep woff /usr/local/nginx/conf/mime.types)
WOFFTWOCHECK=$(grep woff2 /usr/local/nginx/conf/mime.types)

echo $EOTCHECK
echo $TTFCHECK
echo $OTTCHECK
echo $WOFFCHECK
echo $WOFFTWOCHECK

if [[ -z "$$TTFCHECK" && -z "$$OTTCHECK" && -z "$WOFFCHECK" && -z "$WOFFTWOCHECK" ]]; then

	sed -i 's/   application\/octet-stream              eot;//'  /usr/local/nginx/conf/mime.types
	sed -i 's/}//' /usr/local/nginx/conf/mime.types

	cat >> "/usr/local/nginx/conf/mime.types" <<EOF
	    application/vnd.ms-fontobject           eot;
	    application/x-font-ttf                  ttf;
	    font/opentype                           ott;
	    application/font-woff                   woff;
	    application/font-woff2                   woff2;
	    text/cache.manifest       manifest;
	    text/cache.manifest       appcache;
	}
EOF

fi

}