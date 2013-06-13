#! /bin/sh

SSH_PORT=22
SSH_HOST=vpnpeer

OURS=10.0.0.1
PEER=10.0.0.2
REMOTE_NET=192.168.99.0

kill_pppd() {
	/usr/bin/ssh -p ${SSH_PORT} ${SSH_HOST} sudo pkill pppd || :
	sudo pkill pppd || :
}

run_pppd() {
	PTY=$(./ptyrdr /usr/bin/ssh -t -p ${SSH_PORT} ${SSH_HOST} sudo /usr/sbin/pppd noauth nodeflate nobsdcomp)
	echo using ${PTY}..
	sudo pppd ${PTY} ${OURS}:${PEER} noauth
}

setup_routes() {
	IFACE=ppp0

	sudo route delete ${REMOTE_NET}
	sudo route add ${REMOTE_NET} ${PEER} -ifp ${IFACE}
}

set -e

kill_pppd

run_pppd

until setup_routes; do
	sleep 1
done
