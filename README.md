ptyrdr
======

a small program to open a pty.
this can be useful when creating a vpn with ssh and pppd.

expected usage:

	# PTY=$(ptyrdr ssh peer_host pppd noauth)
	# pppd ${PTY} 10.0.0.1:10.0.0.2 noauth
