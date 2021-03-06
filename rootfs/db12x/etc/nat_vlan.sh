# iptables config
iptables --flush
iptables --table nat --flush
iptables --delete-chain
iptables --table nat --delete-chain
iptables -A FORWARD -j ACCEPT -i br0 -o eth0  -m state --state NEW
iptables -A FORWARD -j ACCEPT -i br0 -o ppp0  -m state --state NEW
iptables -A FORWARD -m state --state ESTABLISHED,RELATED  -j ACCEPT
iptables -A POSTROUTING -t nat -o eth0 -j MASQUERADE
iptables -A POSTROUTING -t nat -o ppp0 -j MASQUERADE 
echo 1 >  /proc/sys/net/ipv4/ip_forward

# WAN - LAN/WLAN access
iptables -I PREROUTING -t nat -i eth0 -p tcp --dport 1000:20000 -j DNAT --to 10.10.10.254 -m state --state NEW
iptables -I PREROUTING -t nat -i ppp0 -p tcp --dport 1000:20000 -j DNAT --to 10.10.10.254 -m state --state NEW
iptables -I FORWARD -i eth0 -p tcp --dport 1000:20000 -d 10.10.10.254 -j ACCEPT
iptables -I FORWARD -i ppp0 -p tcp --dport 1000:20000 -d 10.10.10.254 -j ACCEPT

# parent control and access control
iptables -N FORWARD_ACCESSCTRL 
iptables -I FORWARD -p tcp --dport 80 -j FORWARD_ACCESSCTRL
#iptables -I FORWARD -p tcp --dport 53 -j ACCEPT
iptables -I FORWARD -p udp --dport 53 -j ACCEPT
#deal staControl save by zzw
iptables -N control_sta
iptables -I FORWARD -j control_sta

mkdir -p /configure_backup/
mkdir -p /configure_backup/ath/
mkdir -p /configure_backup/ath/iptables/
touch /configure_backup/ath/iptables/parc
touch /configure_backup/ath/iptables/portmap
sh /configure_backup/ath/iptables/parc

iptables -t nat -N PREROUTING_PORTMAP
iptables -t nat -I PREROUTING -j PREROUTING_PORTMAP

#iptables -t nat -N LOCAL_DNS
#iptables -t nat -I PREROUTING -j LOCAL_DNS
#/usr/bin/localdns

iptables -N FORWARD_PORTMAP
iptables -I FORWARD -j FORWARD_PORTMAP

sh /configure_backup/ath/iptables/portmap

iptables -t nat -N PREROUTING_MULTISCR
iptables -t nat -I PREROUTING -j PREROUTING_MULTISCR

