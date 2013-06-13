ptyrdr
======

open a pty and dup it to stdin and stdout.
this can be useful when creating a vpn with ssh and pppd.

an example:

	# pppd $(ptyrdr ssh peer_host pppd noauth) 10.0.0.1:10.0.0.2 noauth
