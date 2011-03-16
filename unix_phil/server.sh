#!/bin/sh

echo Content-type: text/html
echo ""

/bin/cat << EOM
<HTML>
<HEAD><TITLE>File Output: /home/user1/public_html/text-file.txt </TITLE>
</HEAD>
<BODY bgcolor="#cccccc" text="#000000">
<P>
<SMALL>
<PRE>
EOM

/bin/env

cat << EOM
</PRE>
</SMALL>
<P>
</BODY>
</HTML>
EOM
