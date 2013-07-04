ptyrdr
======

Open a PTY and dup it to stdin and stdout.
This can be useful when creating a VPN with ssh and pppd.

An example:

	# pppd $(ptyrdr ssh peer_host pppd noauth) 10.0.0.1:10.0.0.2 noauth
