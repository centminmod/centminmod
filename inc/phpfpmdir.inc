fpmconfdir() {

if [[ ! -d "$PHPFPMCONFDIR" ]]; then
	mkdir -p $PHPFPMCONFDIR
fi

}